#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <map>
#include "Protocol.h"
#include "GameLogic.h"

#pragma comment(lib, "ws2_32.lib")

class GameServer
{
public:
    GameServer();
    ~GameServer();

    bool Start(int port);
    void Stop();

private:
    void AcceptLoop();
    void ClientHandler(SOCKET clientSocket);
    void ProcessPacket(SOCKET clientSocket, PacketHeader* packet);
    bool SendPacket(SOCKET clientSocket, PacketHeader* packet);

    // 게임 로직 관련 메서드
    void HandleLogin(SOCKET clientSocket, C2S_LoginPacket* packet);
    void HandleGameStart(SOCKET clientSocket, C2S_GameStartPacket* packet);
    void HandleMove(SOCKET clientSocket, C2S_MovePacket* packet);
    void HandleAttack(SOCKET clientSocket, C2S_AttackPacket* packet);
    void HandleInteraction(SOCKET clientSocket, C2S_InteractionPacket* packet);

    SOCKET _listenSocket;
    std::map<SOCKET, uint32_t> _clientSocketIdPairs;
    std::mutex _clientsMutex;

    std::atomic<bool> _running;
    std::thread _acceptThread;
    std::thread _physicsThread;

    bool IsGameReady();
    void BroadcastPacket(PacketHeader* packet);

    // 게임 로직
    GameLogic _gameLogic;

    // 물리 시뮬레이션 업데이트
    void PhysicsLoop();
    std::atomic<bool> _physicsRunning;
};