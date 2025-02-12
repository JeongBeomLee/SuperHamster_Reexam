#pragma once
#include "MonoBehaviour.h"
#include "ChestMonsterStateMachine.h"
#include "Player.h"
#include "AttackInfo.h"

class ChestMonster : public MonoBehaviour
{
public:
    ChestMonster();
    ~ChestMonster() override;
    void Awake() override;
    void Start() override;
    void Update() override;

    void SetState(CHEST_MONSTER_STATE newState);
    CHEST_MONSTER_STATE GetCurrentState() const;
    void PlayAnimation(CHEST_MONSTER_STATE state);

    shared_ptr<Animator> GetAnimator();

    void SetTarget(Player* target) { m_target = target; }
    Player* GetTarget() const { return m_target; }

    float GetAttackRange() const { return ATTACK_RANGE; }
    float GetDetectionRange() const { return DETECTION_RANGE; }

    bool IsTargetInAttackRange();
    bool IsTargetInDetectionRange();
    float GetDistanceToTarget();

    void PerformHandAttack();
    void PerformHeadAttack();

    void OnHit(const Event::ProjectileHitEvent& event);
    bool IsAlive() const { return m_health > 0.0f; }

private:
    void CreateComponents();
    void InitializeStateMachine();
    void CheckAttackCollision(const AttackInfo& attackInfo);

private:
    static constexpr float SHORT_ATTACK_RADIUS = 150.0f;
    static constexpr float LONG_ATTACK_RADIUS = 200.0f;
    static constexpr float SHORT_ATTACK_DAMAGE = 10.0f;
    static constexpr float LONG_ATTACK_DAMAGE = 15.0f;
    static constexpr float ATTACK_RANGE = 150.0f;     // 공격 가능 거리
    static constexpr float DETECTION_RANGE = 600.0f;  // 감지 거리
    float m_health = 100.0f;

    ChestMonsterStateMachine m_stateMachine;
    Player* m_target = nullptr;
    size_t m_handHitEventId;
};