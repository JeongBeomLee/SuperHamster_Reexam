#pragma once
#include "PlayerState.h"

class GetUpState : public PlayerState
{
public:
    virtual void Enter(Player* player) override;
    virtual void Update(Player* player, float deltaTime) override;
    virtual void Exit(Player* player) override;
};

