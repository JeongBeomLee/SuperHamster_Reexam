#pragma once
#include "PlayerState.h"

class HitState : public PlayerState
{
public:
    virtual void Enter(Player* player) override;
    virtual void Update(Player* player, float deltaTime) override;
    virtual void Exit(Player* player) override;
};
