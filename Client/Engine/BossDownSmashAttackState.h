#pragma once
#include "BossState.h"
class BossDownSmashAttackState :
    public BossState
{
public:
    virtual void Enter(Boss* boss) override;
    virtual void Update(Boss* boss, float deltaTime) override;
    virtual void Exit(Boss* boss) override;

private:
    bool m_hasCheckedAnimation = false;
    bool m_hasPerformedAttack = false;
    float m_attackRadius = 400.0f;        // 공격 범위
    float m_attackDamage = 30.0f;         // 공격 데미지
    static constexpr float ATTACK_TIMING = 0.4f;  // 애니메이션 40% 지점에서 공격
};

