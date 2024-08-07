#include "GameServer.h"
#include <iostream>
#include <string>

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

    std::lock_guard<std::mutex> lock(_clientsMutex);
    for (const auto& pair : _clientSocketIdPairs) {
        closesocket(pair.first);
    }
    _clientSocketIdPairs.clear();

    if (_listenSocket != INVALID_SOCKET) {
        closesocket(_listenSocket);
        _listenSocket = INVALID_SOCKET;
    }
}

void GameServer::AcceptLoop()
{
    while (_running) {
        SOCKET clientSocket = accept(_listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            if (_running)
                std::cerr << "Accept failed" << std::endl;
            continue;
        }

        std::cout << "New client connected" << std::endl;

        std::lock_guard<std::mutex> lock(_clientsMutex);
        if (_clientSocketIdPairs.size() < 2) {
            uint32_t newId = _clientSocketIdPairs.size() + 1;
            _clientSocketIdPairs[clientSocket] = newId;
            std::cout << "Assigned client ID: " << newId << std::endl;

            std::thread clientThread(&GameServer::ClientHandler, this, clientSocket);
            clientThread.detach();
        }
        else {
            std::cerr << "Max players reached. Rejecting connection." << std::endl;
            closesocket(clientSocket);
        }
    }
}

void GameServer::ClientHandler(SOCKET clientSocket)
{
    char buffer[MAX_PACKET_SIZE];
    while (_running) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cout << "Client disconnected" << std::endl;
            break;
        }

        ProcessPacket(clientSocket, reinterpret_cast<PacketHeader*>(buffer));
    }

    std::lock_guard<std::mutex> lock(_clientsMutex);
    _clientSocketIdPairs.erase(clientSocket);
    closesocket(clientSocket);
}

void GameServer::ProcessPacket(SOCKET clientSocket, PacketHeader* packet)
{
    switch (packet->type) {
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

void GameServer::HandleLogin(SOCKET clientSocket, C2S_LoginPacket* packet)
{
    S2C_LoginResultPacket response;
    response.type = PacketType::S2C_LOGIN_RESULT;
    response.size = sizeof(S2C_LoginResultPacket);

    std::lock_guard<std::mutex> lock(_playersMutex);
    auto it = _clientSocketIdPairs.find(clientSocket);
    if (it != _clientSocketIdPairs.end()) {
        uint32_t playerId = it->second;
        response.success = true;
        response.playerId = playerId;

        PlayerInfo& player = _players[playerId];
        player.playerId = playerId;
        player.isReady = false;

        std::cout << "Player " << playerId << " logged in" << std::endl;
    }
    else {
        response.success = false;
        response.playerId = 0;
        std::cerr << "Login failed: Unknown client" << std::endl;
    }

    SendPacket(clientSocket, &response);
}

void GameServer::HandleGameStart(SOCKET clientSocket, C2S_GameStartPacket* packet)
{
    S2C_GameStartResultPacket response;
    response.type = PacketType::S2C_GAME_START_RESULT;
    response.size = sizeof(S2C_GameStartResultPacket);

    std::lock_guard<std::mutex> lock(_playersMutex);
    auto it = _clientSocketIdPairs.find(clientSocket);
    if (it != _clientSocketIdPairs.end()) {
        uint32_t playerId = it->second;
        PlayerInfo& player = _players[playerId];
        player.isReady = true;

        if (IsGameReady()) {
            response.success = true;
            response.playerNumber = playerId;

            std::cout << "Game started for players " << std::endl;

            // 게임 시작 패킷을 양쪽 클라이언트에 모두 보냄
            BroadcastPacket(&response);
        }
        else {
            response.success = false;
            response.playerNumber = playerId;
            SendPacket(clientSocket, &response);
            std::cout << "Player " << playerId << " is ready. Waiting for other player." << std::endl;
        }
    }
    else {
        response.success = false;
        response.playerNumber = 0;
        SendPacket(clientSocket, &response);
        std::cerr << "Game start failed: Unknown client" << std::endl;
    }
}

void GameServer::HandleMove(SOCKET clientSocket, C2S_MovePacket* packet)
{
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
    BroadcastPacket(&response);
}

void GameServer::HandleAttack(SOCKET clientSocket, C2S_AttackPacket* packet)
{
    S2C_AttackResultPacket response;
    response.type = PacketType::S2C_ATTACK_RESULT;
    response.size = sizeof(S2C_AttackResultPacket);
    response.playerId = packet->playerId;
    response.attackType = packet->attackType;
    response.hit = true;  // 실제로는 충돌 검사 필요
    response.targetId = 0;  // 실제로는 타겟 결정 로직 필요
    response.damage = 10;  // 실제로는 데미지 계산 로직 필요

    // 모든 클라이언트에게 공격 결과 브로드캐스트
    BroadcastPacket(&response);
}

void GameServer::HandleInteraction(SOCKET clientSocket, C2S_InteractionPacket* packet)
{
    S2C_InteractionResultPacket response;
    response.type = PacketType::S2C_INTERACTION_RESULT;
    response.size = sizeof(S2C_InteractionResultPacket);
    response.playerId = packet->playerId;
    response.targetId = packet->targetId;
    response.interactionType = packet->interactionType;
    response.success = true;  // 실제로는 상호작용 가능 여부 확인 필요

    // 모든 클라이언트에게 상호작용 결과 브로드캐스트
    BroadcastPacket(&response);
}

bool GameServer::IsGameReady()
{
    return _players.size() == 2 && _players[1].isReady && _players[2].isReady;
}

void GameServer::BroadcastPacket(PacketHeader* packet)
{
    std::lock_guard<std::mutex> lock(_clientsMutex);
    for (const auto& pair : _clientSocketIdPairs) {
        SendPacket(pair.first, packet);
    }
}