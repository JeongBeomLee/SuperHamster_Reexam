#include "pch.h"
#include "Ghost.h"
#include "GhostIdleState.h"
#include "GhostAttackState.h"
#include "GhostHitState.h"
#include "GhostDeadState.h"
#include "Timer.h"
#include "GameObject.h"
#include "Animator.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Transform.h"
#include "Player.h"
#include "PlayerManager.h"
#include "EventManager.h"
#include "ProjectileManager.h"
#include "TransformAnimator.h"

Ghost::Ghost()
{
}

Ghost::~Ghost()
{
    GET_SINGLE(EventManager)->Unsubscribe<Event::ProjectileHitEvent>(m_handHitEventId);
}

void Ghost::Awake()
{
    InitializeStateMachine();
    CreateComponents();
}

void Ghost::Start()
{
    m_handHitEventId = GET_SINGLE(EventManager)->Subscribe<Event::ProjectileHitEvent>(
        Event::EventCallback<Event::ProjectileHitEvent>(
            [this](const Event::ProjectileHitEvent& event) {
                // target이 이 몬스터의 GameObject인 경우에만 처리
                if (event.target == GetGameObject().get()) {
                    OnHit(event);
                }
            })
    );
}

bool Ghost::IsTargetInAttackRange()
{
    return GetDistanceToTarget() <= ATTACK_RANGE;
}

float Ghost::GetDistanceToTarget()
{
    if (!m_target || !m_target->GetGameObject() || !GetGameObject()) {
        return FLT_MAX;
    }

    Vec3 targetPos = m_target->GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 myPos = GetGameObject()->GetTransform()->GetLocalPosition();
    return (targetPos - myPos).Length();
}

void Ghost::OnHit(const Event::ProjectileHitEvent& event)
{
    // 데미지 처리
    m_health -= ProjectileManager::PROJECTILE_DAMAGE;

    Logger::Instance().Debug("유령이 투사체에 맞음. 남은 체력: {}", m_health);

    if (m_health <= 0) {
        // 체력이 0 이하면 사망 상태로 전환
        SetState(GHOST_STATE::DEAD);
    }
    else {
        // 아니면 피격 상태로 전환
        SetState(GHOST_STATE::HIT);
    }
}

void Ghost::CreateComponents()
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
    controller->SetRadius(70.f);
    controller->SetHeight(190.f);
    controller->Initialize();

    // Movement 컴포넌트 추가
    auto movement = make_shared<CharacterMovement>();
    movement->SetMoveSpeed(300.0f);
    GetGameObject()->AddComponent(movement);
}

void Ghost::InitializeStateMachine()
{
    m_stateMachine.RegisterState<GhostIdleState>(GHOST_STATE::IDLE);
    m_stateMachine.RegisterState<GhostAttackState>(GHOST_STATE::ATTACK);
    m_stateMachine.RegisterState<GhostHitState>(GHOST_STATE::HIT);
    m_stateMachine.RegisterState<GhostDeadState>(GHOST_STATE::DEAD);
    m_stateMachine.SetOwner(this);
    m_stateMachine.ChangeState(GHOST_STATE::IDLE);
}

void Ghost::Update()
{
    float deltaTime = DELTA_TIME;
    m_stateMachine.Update(deltaTime);
}

void Ghost::SetState(GHOST_STATE newState)
{
    m_stateMachine.ChangeState(newState);
}

GHOST_STATE Ghost::GetCurrentState() const
{
    return m_stateMachine.GetCurrentState();
}

void Ghost::PlayAnimation(GHOST_STATE state)
{
    auto animator = GetAnimator();
    if (animator) {
        animator->Play(static_cast<uint32>(state));
    }
}

shared_ptr<Animator> Ghost::GetAnimator()
{
    return GetGameObject()->GetAnimator();
}
