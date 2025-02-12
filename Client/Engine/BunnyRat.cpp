#include "pch.h"
#include "BunnyRat.h"
#include "BunnyRatIdleState.h"
#include "BunnyRatRunState.h"
#include "BunnyRatAttackState.h"
#include "BunnyRatHitState.h"
#include "BunnyRatDeadState.h"
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

BunnyRat::BunnyRat()
{
}

BunnyRat::~BunnyRat()
{
    GET_SINGLE(EventManager)->Unsubscribe<Event::ProjectileHitEvent>(m_handHitEventId);
}

void BunnyRat::Awake()
{
    InitializeStateMachine();
    CreateComponents();
}

void BunnyRat::Start()
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

bool BunnyRat::IsTargetInAttackRange()
{
    return GetDistanceToTarget() <= ATTACK_RANGE;
}

bool BunnyRat::IsTargetInDetectionRange()
{
    return GetDistanceToTarget() <= DETECTION_RANGE;
}

float BunnyRat::GetDistanceToTarget()
{
    if (!m_target || !m_target->GetGameObject() || !GetGameObject()) {
        return FLT_MAX;
    }

    Vec3 targetPos = m_target->GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 myPos = GetGameObject()->GetTransform()->GetLocalPosition();
    return (targetPos - myPos).Length();
}

void BunnyRat::PerformAttack()
{
    Vec3 attackCenter = GetGameObject()->GetTransform()->GetWorldPosition();
    // 전방으로 약간 offset을 주어 공격 위치 조정
    Vec3 forward = GetGameObject()->GetTransform()->GetLook() * -1.f;
    attackCenter += forward * 100.f;

    AttackInfo attackInfo(attackCenter, ATTACK_RADIUS, ATTACK_DAMAGE, GetGameObject().get());
    CheckAttackCollision(attackInfo);
}

void BunnyRat::OnHit(const Event::ProjectileHitEvent& event)
{
    // 데미지 처리
    m_health -= ProjectileManager::PROJECTILE_DAMAGE;

    Logger::Instance().Debug("미믹이 투사체에 맞음. 남은 체력: {}", m_health);

    if (m_health <= 0) {
        // 체력이 0 이하면 사망 상태로 전환
        SetState(BUNNY_RAT_STATE::DEAD);
    }
    else {
        // 아니면 피격 상태로 전환
        SetState(BUNNY_RAT_STATE::HIT);
    }
}

void BunnyRat::CreateComponents()
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

void BunnyRat::InitializeStateMachine()
{
    m_stateMachine.RegisterState<BunnyRatIdleState>(BUNNY_RAT_STATE::IDLE);
    m_stateMachine.RegisterState<BunnyRatRunState>(BUNNY_RAT_STATE::RUN);
    m_stateMachine.RegisterState<BunnyRatAttackState>(BUNNY_RAT_STATE::ATTACK);
    m_stateMachine.RegisterState<BunnyRatHitState>(BUNNY_RAT_STATE::HIT);
    m_stateMachine.RegisterState<BunnyRatDeadState>(BUNNY_RAT_STATE::DEAD);
    m_stateMachine.SetOwner(this);
    m_stateMachine.ChangeState(BUNNY_RAT_STATE::IDLE);
}

void BunnyRat::CheckAttackCollision(const AttackInfo& attackInfo)
{
    const auto& players = GET_SINGLE(PlayerManager)->GetPlayers();

    for (const auto& [playerId, player] : players) {
        if (!player || !player->GetGameObject())
            continue;

        Vec3 playerPos = player->GetGameObject()->GetTransform()->GetWorldPosition();
        float distance = (playerPos - attackInfo.center).Length();

        if (distance <= attackInfo.radius) {
            // 플레이어가 공격 범위 안에 있음
            Logger::Instance().Debug("미믹의 공격이 플레이어 {}에게 적중", playerId);

            // 이벤트 발생
            Event::PlayerHitEvent event(
                player.get(),          // 맞은 플레이어
                attackInfo.attacker,   // 공격자
                attackInfo.damage,     // 데미지
                attackInfo.center      // 타격 위치
            );

            GET_SINGLE(EventManager)->Publish(event);
        }
    }
}

void BunnyRat::Update()
{
    float deltaTime = DELTA_TIME;
    m_stateMachine.Update(deltaTime);
}

void BunnyRat::SetState(BUNNY_RAT_STATE newState)
{
    m_stateMachine.ChangeState(newState);
}

BUNNY_RAT_STATE BunnyRat::GetCurrentState() const
{
    return m_stateMachine.GetCurrentState();
}

void BunnyRat::PlayAnimation(BUNNY_RAT_STATE state)
{
    auto animator = GetAnimator();
    if (animator) {
        animator->Play(static_cast<uint32>(state));
    }
}

shared_ptr<Animator> BunnyRat::GetAnimator()
{
    return GetGameObject()->GetAnimator();
}
