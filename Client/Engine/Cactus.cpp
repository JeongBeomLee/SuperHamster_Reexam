#include "pch.h"
#include "Cactus.h"
#include "CactusSleepState.h"
#include "CactusAwakeState.h"
#include "CactusIdleState.h"
#include "CactusRunState.h"
#include "CactusHandAttackState.h"
#include "CactusHeadAttackState.h"
#include "Timer.h"
#include "GameObject.h"
#include "Animator.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Transform.h"

Cactus::Cactus()
{
}

void Cactus::Awake()
{
    InitializeStateMachine();
    CreateComponents();
}

void Cactus::Start()
{
}

bool Cactus::IsTargetInAttackRange()
{
    return GetDistanceToTarget() <= ATTACK_RANGE;
}

bool Cactus::IsTargetInDetectionRange()
{
    return GetDistanceToTarget() <= DETECTION_RANGE;
}

float Cactus::GetDistanceToTarget()
{
    if (!m_target || !m_target->GetGameObject() || !GetGameObject()) {
        return FLT_MAX;
    }

    Vec3 targetPos = m_target->GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 myPos = GetGameObject()->GetTransform()->GetLocalPosition();
    return (targetPos - myPos).Length();
}

void Cactus::CreateComponents()
{
    // CharacterController 추가
    auto controller = make_shared<CharacterController>();
    GetGameObject()->AddComponent(controller);

    controller->SetCollisionGroup(CollisionGroup::Enemy);
    controller->SetCollisionMask(
        CollisionGroup::Default | 
        CollisionGroup::Ground |
        CollisionGroup::Obstacle | 
        CollisionGroup::Player |
        CollisionGroup::Enemy | 
        CollisionGroup::Projectile
    );
    controller->Initialize();

    // Movement 컴포넌트 추가
    auto movement = make_shared<CharacterMovement>();
	movement->SetMoveSpeed(0.0f);
    GetGameObject()->AddComponent(movement);
}

void Cactus::InitializeStateMachine()
{
    m_stateMachine.RegisterState<CactusSleepState>(CACTUS_STATE::SLEEP);
    m_stateMachine.RegisterState<CactusAwakeState>(CACTUS_STATE::AWAKE);
    m_stateMachine.RegisterState<CactusIdleState>(CACTUS_STATE::IDLE);
    m_stateMachine.RegisterState<CactusRunState>(CACTUS_STATE::RUN);
    m_stateMachine.RegisterState<CactusHandAttackState>(CACTUS_STATE::HAND_ATTACK);
    m_stateMachine.RegisterState<CactusHeadAttackState>(CACTUS_STATE::HEAD_ATTACK);
    m_stateMachine.SetOwner(this);
    m_stateMachine.ChangeState(CACTUS_STATE::SLEEP);
}

void Cactus::Update() 
{
    float deltaTime = DELTA_TIME;
    m_stateMachine.Update(deltaTime);
}

void Cactus::SetState(CACTUS_STATE newState) 
{
    m_stateMachine.ChangeState(newState);
}

CACTUS_STATE Cactus::GetCurrentState() const 
{
    return m_stateMachine.GetCurrentState();
}

void Cactus::PlayAnimation(CACTUS_STATE state) 
{
    auto animator = GetAnimator();
    if (animator) {
        animator->Play(static_cast<uint32>(state));
    }
}

shared_ptr<Animator> Cactus::GetAnimator() 
{
    return GetGameObject()->GetAnimator();
}