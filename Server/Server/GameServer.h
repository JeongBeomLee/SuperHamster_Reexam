#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include "Protocol.h"

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

    // ���� ���� ���� �޼���
    void HandleLogin(SOCKET clientSocket, C2S_LoginPacket* packet);
    void HandleGameStart(SOCKET clientSocket, C2S_GameStartPacket* packet);
    void HandleMove(SOCKET clientSocket, C2S_MovePacket* packet);
    void HandleAttack(SOCKET clientSocket, C2S_AttackPacket* packet);
    void HandleInteraction(SOCKET clientSocket, C2S_InteractionPacket* packet);

    SOCKET _listenSocket;
    std::vector<SOCKET> _clientSockets;
    std::mutex _clientsMutex;

    std::atomic<bool> _running;
    std::thread _acceptThread;

    // ���� ���� ����
    struct PlayerInfo
    {
        uint32_t playerId;
        float posX, posY, posZ;
        // �߰� �÷��̾� ����...
    };
    std::vector<PlayerInfo> _players;
    std::mutex _playersMutex;
};