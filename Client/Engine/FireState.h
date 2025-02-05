#pragma once
#include "PlayerState.h"

class FireState : public PlayerState
{
public:
    virtual void Enter(Player* player) override;
    virtual void Update(Player* player, float deltaTime) override;
    virtual void Exit(Player* player) override;

private:
    bool m_hasCheckedAnimation = false;
};


