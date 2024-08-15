#pragma once
#include "PlayerStateMachine.h"

class NetworkManager
{
public:
    NetworkManager();
    ~NetworkManager();

    bool Connect(const std::string& serverIP, int port);
    void Disconnect();
    bool SendPacket(PacketHeader* packet);
    void Update();

    // ��Ŷ �ڵ鷯 ��� �Լ�
    using PacketHandler = std::function<void(PacketHeader*)>;
    void RegisterHandler(PacketType type, PacketHandler handler);

    void SendStateUpdate(uint32_t playerId, PLAYER_STATE newState);

private:
    void RecvThread();
    void ProcessPacket(PacketHeader* packet);

    SOCKET _socket;
    std::thread _recvThread;
    std::mutex _sendQueueMutex;
    std::queue<PacketHeader*> _sendQueue;
    bool _isConnected;

    std::unordered_map<PacketType, PacketHandler> _packetHandlers;
};