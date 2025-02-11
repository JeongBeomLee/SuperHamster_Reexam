#pragma once
#include "MonoBehaviour.h"
#include "CactusStateMachine.h"
#include "Player.h"

class Cactus : public MonoBehaviour 
{
public:
	Cactus();
	void Awake() override;
    void Start() override;
    void Update() override;

    void SetState(CACTUS_STATE newState);
    CACTUS_STATE GetCurrentState() const;
    void PlayAnimation(CACTUS_STATE state);

    shared_ptr<Animator> GetAnimator();

    void SetTarget(Player* target) { m_target = target; }
    Player* GetTarget() const { return m_target; }

    float GetAttackRange() const { return ATTACK_RANGE; }
    float GetDetectionRange() const { return DETECTION_RANGE; }

    bool IsTargetInAttackRange();
    bool IsTargetInDetectionRange();
    float GetDistanceToTarget();

private:
    void CreateComponents();
    void InitializeStateMachine();

private:
    CactusStateMachine m_stateMachine;
    Player* m_target = nullptr;
    static constexpr float ATTACK_RANGE = 150.0f;     // 공격 가능 거리
    static constexpr float DETECTION_RANGE = 500.0f;  // 감지 거리
};