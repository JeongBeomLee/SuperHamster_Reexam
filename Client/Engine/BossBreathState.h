#pragma once
#include "BossState.h"
class BossBreathState :
    public BossState
{
public:
    virtual void Enter(Boss* boss) override;
    virtual void Update(Boss* boss, float deltaTime) override;
    virtual void Exit(Boss* boss) override;

private:
    float m_stateTimer = 0.0f;
    float m_attackTimer = 0.0f;
    int m_currentAttackIndex = 0;
    bool m_hasFinalAttack = false;

    static constexpr float PREPARE_TIME = 2.0f;     // 브레스 시작 전 대기 시간
    static constexpr float ATTACK_INTERVAL = 0.25f; // 공격 간격
    static constexpr float FINAL_ATTACK_DELAY = 1.0f; // 최종 공격 전 대기 시간

    void PerformBreathAttack(Boss* boss, const Vec3& center, bool isFinal);
};

