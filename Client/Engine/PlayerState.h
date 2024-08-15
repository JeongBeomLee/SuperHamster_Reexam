#pragma once
#include "PlayerStateMachine.h"

class Player;
class PlayerState
{
public:
    virtual ~PlayerState() = default;
    virtual void Enter(Player* player) = 0;
    virtual void Update(Player* player, float deltaTime) = 0;
    virtual void Exit(Player* player) = 0;
};