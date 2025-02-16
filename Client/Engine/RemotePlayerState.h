#pragma once
#include "Player.h"
class RemotePlayerState {
public:
    virtual ~RemotePlayerState() = default;
    virtual void Enter(Player* player) = 0;
    virtual void Update(Player* player, float deltaTime) = 0;
    virtual void Exit(Player* player) = 0;
    virtual void ProcessNetworkInput(Player* player, const NetworkInputData& inputData) = 0;
};