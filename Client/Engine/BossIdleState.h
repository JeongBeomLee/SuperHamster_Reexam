#pragma once
#include "BossState.h"
class BossIdleState :
    public BossState
{
public:
    virtual void Enter(Boss* boss) override;
    virtual void Update(Boss* boss, float deltaTime) override;
    virtual void Exit(Boss* boss) override;

private:
    float m_stateTimer = 0.0f;
    bool m_isInitialIdle = true;
    bool UpdateTarget(Boss* boss);
};

