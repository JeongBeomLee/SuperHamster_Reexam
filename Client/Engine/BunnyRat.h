#pragma once
#include "MonoBehaviour.h"
#include "BunnyRatStateMachine.h"
#include "Player.h"
#include "AttackInfo.h"

class BunnyRat : public MonoBehaviour
{
public:
    BunnyRat();
    ~BunnyRat() override;
    void Awake() override;
    void Start() override;
    void Update() override;

    void SetState(BUNNY_RAT_STATE newState);
    BUNNY_RAT_STATE GetCurrentState() const;
    void PlayAnimation(BUNNY_RAT_STATE state);

    shared_ptr<Animator> GetAnimator();

    void SetTarget(Player* target) { m_target = target; }
    Player* GetTarget() const { return m_target; }

    float GetAttackRange() const { return ATTACK_RANGE; }
    float GetDetectionRange() const { return DETECTION_RANGE; }

    bool IsTargetInAttackRange();
    bool IsTargetInDetectionRange();
    float GetDistanceToTarget();

    void PerformAttack();

    void OnHit(const Event::ProjectileHitEvent& event);
    bool IsAlive() const { return m_health > 0.0f; }

private:
    void CreateComponents();
    void InitializeStateMachine();
    void CheckAttackCollision(const AttackInfo& attackInfo);

private:
    static constexpr float ATTACK_RADIUS = 150.0f;
    static constexpr float ATTACK_DAMAGE = 10.0f;
    static constexpr float ATTACK_RANGE = 150.0f;     // ���� ���� �Ÿ�
    static constexpr float DETECTION_RANGE = 600.0f;  // ���� �Ÿ�
    float m_health = 100.0f;

    BunnyRatStateMachine m_stateMachine;
    Player* m_target = nullptr;
    size_t m_handHitEventId;
};