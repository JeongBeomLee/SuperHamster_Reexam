#pragma once
#include "BossState.h"
class BossLeftHandAttackState :
    public BossState
{
public:
    virtual void Enter(Boss* boss) override;
    virtual void Update(Boss* boss, float deltaTime) override;
    virtual void Exit(Boss* boss) override;

private:
    bool m_hasCheckedAnimation = false;
    bool m_hasPerformedAttack = false;
    static constexpr float ATTACK_TIMING = 0.5f;  // 애니메이션 50% 지점에서 공격
};

