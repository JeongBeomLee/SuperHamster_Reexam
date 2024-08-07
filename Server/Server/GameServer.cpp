#include "GameServer.h"
#include <iostream>

GameServer::GameServer() : _listenSocket(INVALID_SOCKET), _running(false)
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

GameServer::~GameServer()
{
    Stop();
    WSACleanup();
}

bool GameServer::Start(int port)
{
    _listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_listenSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(_listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(_listenSocket);
        return false;
    }

    if (listen(_listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(_listenSocket);
        return false;
    }

    _running = true;
    _acceptThread = std::thread(&GameServer::AcceptLoop, this);

    std::cout << "Server started on port " << port << std::endl;
    return true;
}

void GameServer::Stop()
{
    _running = false;
    if (_acceptThread.joinable())
        _acceptThread.join();

    for (SOCKET clientSocket : _clientSockets)
    {
        closesocket(clientSocket);
    }
    _clientSockets.clear();

    if (_listenSocket != INVALID_SOCKET)
    {
        closesocket(_listenSocket);
        _listenSocket = INVALID_SOCKET;
    }
}

void GameServer::AcceptLoop()
{
    while (_running)
    {
        SOCKET clientSocket = accept(_listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET)
        {
            if (_running)
                std::cerr << "Accept failed" << std::endl;
            continue;
        }

        std::cout << "New client connected" << std::endl;

        std::lock_guard<std::mutex> lock(_clientsMutex);
        _clientSockets.push_back(clientSocket);

        std::thread clientThread(&GameServer::ClientHandler, this, clientSocket);
        clientThread.detach();  // 스레드 분리(독립 실행)
    }
}

void GameServer::ClientHandler(SOCKET clientSocket)
{
    char buffer[MAX_PACKET_SIZE];
    while (_running)
    {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0)
        {
            std::cout << "Client disconnected" << std::endl;
            break;
        }

        ProcessPacket(clientSocket, reinterpret_cast<PacketHeader*>(buffer));
    }

    std::lock_guard<std::mutex> lock(_clientsMutex);
    _clientSockets.erase(std::remove(_clientSockets.begin(), _clientSockets.end(), clientSocket), _clientSockets.end());
    closesocket(clientSocket);
}

void GameServer::ProcessPacket(SOCKET clientSocket, PacketHeader* packet)
{
    switch (packet->type)
    {
    case PacketType::C2S_LOGIN:
        HandleLogin(clientSocket, reinterpret_cast<C2S_LoginPacket*>(packet));
        break;
    case PacketType::C2S_GAME_START:
        HandleGameStart(clientSocket, reinterpret_cast<C2S_GameStartPacket*>(packet));
        break;
    case PacketType::C2S_MOVE:
        HandleMove(clientSocket, reinterpret_cast<C2S_MovePacket*>(packet));
        break;
    case PacketType::C2S_ATTACK:
        HandleAttack(clientSocket, reinterpret_cast<C2S_AttackPacket*>(packet));
        break;
    case PacketType::C2S_INTERACTION:
        HandleInteraction(clientSocket, reinterpret_cast<C2S_InteractionPacket*>(packet));
        break;
    default:
        std::cerr << "Unknown packet type: " << static_cast<int>(packet->type) << std::endl;
        break;
    }
}

bool GameServer::SendPacket(SOCKET clientSocket, PacketHeader* packet)
{
    int bytesSent = send(clientSocket, reinterpret_cast<char*>(packet), packet->size, 0);
    return bytesSent == packet->size;
}

// 게임 로직 관련 메서드 구현
void GameServer::HandleLogin(SOCKET clientSocket, C2S_LoginPacket* packet)
{
    // 로그인 처리 로직
    S2C_LoginResultPacket response;
    response.type = PacketType::S2C_LOGIN_RESULT;
    response.size = sizeof(S2C_LoginResultPacket);
    response.success = true;  // 실제로는 검증 로직이 필요
    response.playerId = 1;    // 실제로는 유니크한 ID 할당 필요

    SendPacket(clientSocket, &response);
}

void GameServer::HandleGameStart(SOCKET clientSocket, C2S_GameStartPacket* packet)
{
    // 게임 시작 처리 로직
    S2C_GameStartResultPacket response;
    response.type = PacketType::S2C_GAME_START_RESULT;
    response.size = sizeof(S2C_GameStartResultPacket);
    response.success = true;
    response.gameId = 1;  // 실제로는 유니크한 게임 ID 할당 필요
    response.playerNumber = 1;  // 첫 번째 플레이어 또는 두 번째 플레이어

    SendPacket(clientSocket, &response);
}

void GameServer::HandleMove(SOCKET clientSocket, C2S_MovePacket* packet)
{
    // 이동 처리 로직
    S2C_MoveResultPacket response;
    response.type = PacketType::S2C_MOVE_RESULT;
    response.size = sizeof(S2C_MoveResultPacket);
    response.playerId = packet->playerId;
    response.posX = packet->posX;
    response.posY = packet->posY;
    response.posZ = packet->posZ;
    response.dirX = packet->dirX;
    response.dirY = packet->dirY;
    response.dirZ = packet->dirZ;

    // 모든 클라이언트에게 이동 결과 브로드캐스트
    std::lock_guard<std::mutex> lock(_clientsMutex);
    for (SOCKET socket : _clientSockets)
    {
        SendPacket(socket, &response);
    }
}

void GameServer::HandleAttack(SOCKET clientSocket, C2S_AttackPacket* packet)
{
    // 공격 처리 로직
    S2C_AttackResultPacket response;
    response.type = PacketType::S2C_ATTACK_RESULT;
    response.size = sizeof(S2C_AttackResultPacket);
    response.playerId = packet->playerId;
    response.attackType = packet->attackType;
    response.hit = true;  // 실제로는 충돌 검사 필요
    response.targetId = 0;  // 실제로는 타겟 결정 로직 필요
    response.damage = 10;  // 실제로는 데미지 계산 로직 필요

    // 모든 클라이언트에게 공격 결과 브로드캐스트
    std::lock_guard<std::mutex> lock(_clientsMutex);
    for (SOCKET socket : _clientSockets)
    {
        SendPacket(socket, &response);
    }
}

void GameServer::HandleInteraction(SOCKET clientSocket, C2S_InteractionPacket* packet)
{
    // 상호작용 처리 로직
    S2C_InteractionResultPacket response;
    response.type = PacketType::S2C_INTERACTION_RESULT;
    response.size = sizeof(S2C_InteractionResultPacket);
    response.playerId = packet->playerId;
    response.targetId = packet->targetId;
    response.interactionType = packet->interactionType;
    response.success = true;  // 실제로는 상호작용 가능 여부 확인 필요

    // 모든 클라이언트에게 상호작용 결과 브로드캐스트
    std::lock_guard<std::mutex> lock(_clientsMutex);
    for (SOCKET socket : _clientSockets)
    {
        SendPacket(socket, &response);
    }
}