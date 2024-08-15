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
    _physicsRunning = true;
    _acceptThread = std::thread(&GameServer::AcceptLoop, this);
    _physicsThread = std::thread(&GameServer::PhysicsLoop, this);

    _gameLogic.StartGame(); // 테스트: 바로 게임 시작

    std::cout << "Server started on port " << port << std::endl;
    return true;
}

void GameServer::Stop()
{
    _running = false;
    _physicsRunning = false;
    if (_acceptThread.joinable())
        _acceptThread.join();
    if (_physicsThread.joinable())
        _physicsThread.join();

    std::lock_guard<std::mutex> lock(_clientsMutex);
    for (const auto& pair : _clientSocketIdPairs) {
        closesocket(pair.first);
    }
    _clientSocketIdPairs.clear();

    if (_listenSocket != INVALID_SOCKET) {
        closesocket(_listenSocket);
        _listenSocket = INVALID_SOCKET;
    }

    std::cout << "Server stopped" << std::endl;
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
            uint32_t newId = _clientSocketIdPairs.size();
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
			std::cout << "Client " << _clientSocketIdPairs[clientSocket] << " disconnected" << std::endl;
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
    case PacketType::C2S_UPDATE_PLAYER_STATE:
        HandleUpdatePlayerState(clientSocket, reinterpret_cast<C2S_UpdatePlayerStatePacket*>(packet));
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

    std::lock_guard<std::mutex> lock(_clientsMutex);
    auto it = _clientSocketIdPairs.find(clientSocket);
    if (it != _clientSocketIdPairs.end()) {
        uint32_t playerId = it->second;
        response.success = true;
        response.playerId = playerId;

        std::cout << "Player " << playerId << " logged in" << std::endl;
    }
    else {
        response.success = false;
        response.playerId = -1;
        std::cerr << "Login failed: Unknown client" << std::endl;
    }
    
    SendPacket(clientSocket, &response);
}

void GameServer::HandleGameStart(SOCKET clientSocket, C2S_GameStartPacket* packet)
{
    S2C_GameStartResultPacket response;
    response.type = PacketType::S2C_GAME_START_RESULT;
    response.size = sizeof(S2C_GameStartResultPacket);

    std::lock_guard<std::mutex> lock(_clientsMutex);
    auto it = _clientSocketIdPairs.find(clientSocket);
    if (it != _clientSocketIdPairs.end()) {
        uint32_t playerId = it->second;
        if (IsGameReady()) {
            response.success = true;
            response.playerNumber = playerId;
            //_gameLogic.StartGame();

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
    uint32_t playerId = _clientSocketIdPairs[clientSocket];
    physx::PxVec3 moveDir(packet->dirX, 0, packet->dirZ);
    moveDir.normalize();

    if (_gameLogic.MovePlayer(playerId, moveDir)) {
        S2C_MoveResultPacket response;
        response.type = PacketType::S2C_MOVE_RESULT;
        response.size = sizeof(S2C_MoveResultPacket);
        response.playerId = playerId;

        physx::PxExtendedVec3 newPosition = _gameLogic._playerPhysics[playerId].controller->getPosition();
        response.posX = newPosition.x;
        response.posY = newPosition.y;
        response.posZ = newPosition.z;
        response.dirX = moveDir.x;
        response.dirY = moveDir.y;
        response.dirZ = moveDir.z;

        BroadcastPacket(&response);
    }
}

void GameServer::HandleAttack(SOCKET clientSocket, C2S_AttackPacket* packet)
{
    uint32_t playerId = _clientSocketIdPairs[clientSocket];
    auto result = _gameLogic.ProcessAttack(playerId, packet->attackType);

    S2C_AttackResultPacket response;
    response.type = PacketType::S2C_ATTACK_RESULT;
    response.size = sizeof(S2C_AttackResultPacket);
    response.playerId = playerId;
    response.attackType = packet->attackType;
    response.hit = result.hit;
    response.targetId = result.targetId;
    response.damage = result.damage;

    BroadcastPacket(&response);
}

void GameServer::HandleInteraction(SOCKET clientSocket, C2S_InteractionPacket* packet)
{
    uint32_t playerId = _clientSocketIdPairs[clientSocket];
    bool success = _gameLogic.ProcessInteraction(playerId, packet->targetId, packet->interactionType);

    S2C_InteractionResultPacket response;
    response.type = PacketType::S2C_INTERACTION_RESULT;
    response.size = sizeof(S2C_InteractionResultPacket);
    response.playerId = playerId;
    response.targetId = packet->targetId;
    response.interactionType = packet->interactionType;
    response.success = success;

    BroadcastPacket(&response);
}

void GameServer::HandleUpdatePlayerState(SOCKET clientSocket, C2S_UpdatePlayerStatePacket* packet)
{
	uint32_t playerId = _clientSocketIdPairs[clientSocket];
    PLAYER_STATE newState = static_cast<PLAYER_STATE>(packet->newState);

	_gameLogic.UpdatePlayerState(playerId, newState);

    // 다른 클라이언트에게 상태 변경을 브로드캐스트
    S2C_UpdatePlayerStatePacket response;
    response.type = PacketType::S2C_UPDATE_PLAYER_STATE;
    response.size = sizeof(S2C_UpdatePlayerStatePacket);
    response.playerId = playerId;
    response.newState = packet->newState;

    BroadcastPacket(&response);
}

bool GameServer::IsGameReady()
{
    return _clientSocketIdPairs.size() == 2;
}

void GameServer::BroadcastPacket(PacketHeader* packet)
{
    std::lock_guard<std::mutex> lock(_clientsMutex);
    for (const auto& pair : _clientSocketIdPairs) {
        SendPacket(pair.first, packet);
    }
}

void GameServer::PhysicsLoop()
{
    const float FIXED_TIME_STEP = 1.0f / 60.0f; // 60 FPS
    float accumulator = 0.0f;
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (_physicsRunning)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        accumulator += deltaTime;

        while (accumulator >= FIXED_TIME_STEP)
        {
            _gameLogic.Update(FIXED_TIME_STEP);
            accumulator -= FIXED_TIME_STEP;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}