#pragma once
#include "MonoBehaviour.h"
#include "GhostStateMachine.h"
#include "Player.h"
#include "AttackInfo.h"

class Ghost : public MonoBehaviour
{
public:
    Ghost();
    ~Ghost() override;
    void Awake() override;
    void Start() override;
    void Update() override;

    void SetState(GHOST_STATE newState);
    GHOST_STATE GetCurrentState() const;
    void PlayAnimation(GHOST_STATE state);

    shared_ptr<Animator> GetAnimator();

    void SetTarget(Player* target) { m_target = target; }
    Player* GetTarget() const { return m_target; }

    float GetAttackRange() const { return ATTACK_RANGE; }
	float GetPullForce() const { return PULL_FORCE; }

    bool IsTargetInAttackRange();
    float GetDistanceToTarget();

    void OnHit(const Event::ProjectileHitEvent& event);
    bool IsAlive() const { return m_health > 0.0f; }

private:
    void CreateComponents();
    void InitializeStateMachine();

private:
    static constexpr float ATTACK_RANGE = 700.0f;        // 공격 감지 거리
    static constexpr float PULL_FORCE = 300.0f;          // 당기는 힘의 크기
    float m_health = 100.0f;

    GhostStateMachine m_stateMachine;
    Player* m_target = nullptr;
    size_t m_handHitEventId;
};