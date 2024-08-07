#pragma once
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