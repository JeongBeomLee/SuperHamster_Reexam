#pragma once
#include "MonoBehaviour.h"
#include "BossStateMachine.h"
#include "Player.h"
#include "AttackInfo.h"

class Boss : public MonoBehaviour
{
public:
    Boss();
    ~Boss() override;
    void Awake() override;
    void Start() override;
    void Update() override;

    void SetState(BOSS_STATE newState);
    BOSS_STATE GetCurrentState() const;
    void PlayAnimation(BOSS_STATE state);

    shared_ptr<Animator> GetAnimator();

    void SetTarget(Player* target) { m_target = target; }
    Player* GetTarget() const { return m_target; }

    BOSS_PHASE GetCurrentPhase() const { return m_currentPhase; }
    bool IsPhaseTransitionRequired() const;
    void TransitionToNextPhase();

    float GetAttackRange() const { return ATTACK_RANGE; }
    void IncrementNormalAttackCount();
    void IncrementPatternCycleCount();

    bool IsTargetInAttackRange();
    float GetDistanceToTarget();
	float GetChargeSpeed() const { return CHARGE_SPEED; }
	float GetRunSpeed() const { return RUN_SPEED; }
	const Vec3& GetBreathPosition() const { return BREATH_POSITION; }
	const std::vector<Vec3>& GetBreathAttackPositions() const { return m_breathAttackPositions; }

    void OnHit(const Event::ProjectileHitEvent& event);
    bool IsAlive() const { return m_health > 0.0f; }

private:
    void CreateComponents();
    void InitializeStateMachine();
    void InitializeBreathPositions();
    bool ShouldStartBreathPattern() const;
    void UpdatePhase();
    void CheckPatternTransition();

private:
    BossStateMachine m_stateMachine;
    Player* m_target = nullptr;
    size_t m_handHitEventId;

    float m_health = 510.0f;
    float m_maxHealth = 1000.0f;
    BOSS_PHASE m_currentPhase = BOSS_PHASE::NONE;

    int m_normalAttackCount = 0;    // 일반 공격 패턴 카운트
    int m_patternCycleCount = 0;    // 패턴 사이클 카운트

    float m_stateTimer = 0.0f;      // 상태 지속 시간
    float m_patternTimer = 0.0f;    // 패턴 타이머

    static constexpr float ATTACK_RANGE = 200.0f;
    static constexpr float RUN_SPEED = 400.0f;
    static constexpr float CHARGE_SPEED = 1000.0f;
    static constexpr Vec3 BREATH_POSITION = Vec3(-5.0f, 335.0f, -4000.0f);

    std::vector<Vec3> m_breathAttackPositions;  // 브레스 공격 위치들
    int m_currentBreathIndex = 0;
};