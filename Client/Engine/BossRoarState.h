#pragma once
#include "BossState.h"
class BossRoarState :
    public BossState
{
public:
    virtual void Enter(Boss* boss) override;
    virtual void Update(Boss* boss, float deltaTime) override;
    virtual void Exit(Boss* boss) override;

private:
    float m_roarTimer = 0.0f;
    static constexpr float ROAR_DURATION = 2.0f;
    bool m_phaseTransitionProcessed = false;
};

