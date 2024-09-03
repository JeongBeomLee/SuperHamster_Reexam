#include "pch.h"
#include "NetworkManager.h"

NetworkManager::NetworkManager() : _socket(INVALID_SOCKET), _isConnected(false)
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

NetworkManager::~NetworkManager()
{
    Disconnect();
    WSACleanup();
}

bool NetworkManager::Connect(const std::string& serverIP, int port)
{
    int DelayZeroOpt = 1;
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&DelayZeroOpt, sizeof(DelayZeroOpt));
    if (_socket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);
    serverAddr.sin_port = htons(port);

    if (connect(_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to connect to server" << std::endl;
        closesocket(_socket);
        return false;
    }

    _isConnected = true;
    _recvThread = std::thread(&NetworkManager::RecvThread, this);

    std::cout << "Connected to server" << std::endl;
    return true;
}

void NetworkManager::Disconnect()
{
    _isConnected = false;
    if (_recvThread.joinable())
        _recvThread.join();

    if (_socket != INVALID_SOCKET)
    {
        closesocket(_socket);
        _socket = INVALID_SOCKET;
    }
}

bool NetworkManager::SendPacket(PacketHeader* packet)
{
    if (!_isConnected)
        return false;

    std::lock_guard<std::mutex> lock(_sendQueueMutex);
    _sendQueue.push(packet);
    return true;
}

void NetworkManager::Update()
{
    if (!_isConnected)
        return;

    std::lock_guard<std::mutex> lock(_sendQueueMutex);
    while (!_sendQueue.empty())
    {
        PacketHeader* packet = _sendQueue.front();
        _sendQueue.pop();

        int bytesSent = send(_socket, reinterpret_cast<char*>(packet), packet->size, 0);
        if (bytesSent == SOCKET_ERROR)
        {
            std::cerr << "Failed to send packet" << std::endl;
            // 에러 처리
        }
        delete packet;
    }
}

void NetworkManager::RegisterHandler(PacketType type, PacketHandler handler)
{
    _packetHandlers[type] = handler;
}

void NetworkManager::SendStateUpdate(uint32_t playerId, PLAYER_STATE newState)
{
    C2S_UpdatePlayerStatePacket* packet = new C2S_UpdatePlayerStatePacket();
    packet->type = PacketType::C2S_UPDATE_PLAYER_STATE;
    packet->size = sizeof(C2S_UpdatePlayerStatePacket);
    packet->playerId = playerId;
    packet->newState = static_cast<uint32_t>(newState);

    SendPacket(packet);
}

void NetworkManager::RecvThread()
{
    char buffer[MAX_PACKET_SIZE];
    while (_isConnected)
    {
        int bytesReceived = recv(_socket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0)
        {
            if (_isConnected)
            {
                std::cerr << "Disconnected from server" << std::endl;
                _isConnected = false;
            }
            break;
        }

        ProcessPacket(reinterpret_cast<PacketHeader*>(buffer));
    }
}

void NetworkManager::ProcessPacket(PacketHeader* packet)
{
    auto it = _packetHandlers.find(packet->type);
    if (it != _packetHandlers.end())
    {
        it->second(packet);
    }
    else
    {
        std::cerr << "Unknown packet type: " << static_cast<int>(packet->type) << std::endl;
    }
}