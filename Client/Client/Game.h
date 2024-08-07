#pragma once
#include "NetworkManager.h"

class Game
{
public:
	void Init(const WindowInfo& info);
	void Update();

private:
    std::unique_ptr<NetworkManager> _networkManager;

    void NetworkUpdate();
    void RegisterPacketHandlers();

    // 패킷 핸들러 함수들
    void Handle_S2C_LOGIN_RESULT(PacketHeader* packet);
    void Handle_S2C_GAME_START_RESULT(PacketHeader* packet);
    void Handle_S2C_MOVE_RESULT(PacketHeader* packet);
    void Handle_S2C_ATTACK_RESULT(PacketHeader* packet);
    void Handle_S2C_INTERACTION_RESULT(PacketHeader* packet);
};

