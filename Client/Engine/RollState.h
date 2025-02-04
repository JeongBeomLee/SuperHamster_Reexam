#pragma once
#include "PlayerState.h"

class RollState : public PlayerState
{
public:
    virtual void Enter(Player* player) override;
    virtual void Update(Player* player, float deltaTime) override;
    virtual void Exit(Player* player) override;

private:
    Vec3 m_rollDirection = Vec3::Zero;
    float m_rollTimer = 0.0f;
};
