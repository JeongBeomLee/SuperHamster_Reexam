#pragma once
#include "BossState.h"
class BossChargeState :
    public BossState
{
public:
    virtual void Enter(Boss* boss) override;
    virtual void Update(Boss* boss, float deltaTime) override;
    virtual void Exit(Boss* boss) override;

private:
    Vec3 m_chargeDirection;
    Vec3 m_targetPosition;
    bool m_hasStartedCharge = false;
    float m_prepareTimer = 0.0f;

    static constexpr float PREPARE_TIME = 0.2f;  // 돌진 전 준비 시간
};

