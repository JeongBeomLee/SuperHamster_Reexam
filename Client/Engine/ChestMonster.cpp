#include "pch.h"
#include "ChestMonster.h"
#include "ChestMonsterSleepState.h"
#include "ChestMonsterAwakeState.h"
#include "ChestMonsterIdleState.h"
#include "ChestMonsterRunState.h"
#include "ChestMonsterShortAttackState.h"
#include "ChestMonsterLongAttackState.h"
#include "ChestMonsterHitState.h"
#include "ChestMonsterDeadState.h"
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

ChestMonster::ChestMonster()
{
}

ChestMonster::~ChestMonster()
{
    GET_SINGLE(EventManager)->Unsubscribe<Event::ProjectileHitEvent>(m_handHitEventId);
}

void ChestMonster::Awake()
{
    InitializeStateMachine();
    CreateComponents();
}

void ChestMonster::Start()
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

bool ChestMonster::IsTargetInAttackRange()
{
    return GetDistanceToTarget() <= ATTACK_RANGE;
}

bool ChestMonster::IsTargetInDetectionRange()
{
    return GetDistanceToTarget() <= DETECTION_RANGE;
}

float ChestMonster::GetDistanceToTarget()
{
    if (!m_target || !m_target->GetGameObject() || !GetGameObject()) {
        return FLT_MAX;
    }

    Vec3 targetPos = m_target->GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 myPos = GetGameObject()->GetTransform()->GetLocalPosition();
    return (targetPos - myPos).Length();
}

void ChestMonster::PerformHandAttack()
{
    Vec3 attackCenter = GetGameObject()->GetTransform()->GetWorldPosition();
    // 전방으로 약간 offset을 주어 공격 위치 조정
    Vec3 forward = GetGameObject()->GetTransform()->GetLook() * -1.f;
    attackCenter += forward * 100.f;

    AttackInfo attackInfo(attackCenter, SHORT_ATTACK_RADIUS, SHORT_ATTACK_DAMAGE, GetGameObject().get());
    CheckAttackCollision(attackInfo);
}

void ChestMonster::PerformHeadAttack()
{
    Vec3 attackCenter = GetGameObject()->GetTransform()->GetWorldPosition();
    Vec3 forward = GetGameObject()->GetTransform()->GetLook() * -1.f;
    attackCenter += forward * 200.f;

    AttackInfo attackInfo(attackCenter, LONG_ATTACK_RADIUS, LONG_ATTACK_DAMAGE, GetGameObject().get());
    CheckAttackCollision(attackInfo);
}

void ChestMonster::OnHit(const Event::ProjectileHitEvent& event)
{
    // 데미지 처리
    m_health -= ProjectileManager::PROJECTILE_DAMAGE;

    Logger::Instance().Debug("미믹이 투사체에 맞음. 남은 체력: {}", m_health);

    if (m_health <= 0) {
        // 체력이 0 이하면 사망 상태로 전환
        SetState(CHEST_MONSTER_STATE::DEAD);
    }
    else {
        // 아니면 피격 상태로 전환
        SetState(CHEST_MONSTER_STATE::HIT);
    }
}

void ChestMonster::CreateComponents()
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
    controller->SetRadius(60.f);
    controller->SetHeight(190.f);
    controller->Initialize();

    // Movement 컴포넌트 추가
    auto movement = make_shared<CharacterMovement>();
    movement->SetMoveSpeed(100.0f);
    GetGameObject()->AddComponent(movement);
}

void ChestMonster::InitializeStateMachine()
{
    m_stateMachine.RegisterState<ChestMonsterSleepState>(CHEST_MONSTER_STATE::SLEEP);
    m_stateMachine.RegisterState<ChestMonsterAwakeState>(CHEST_MONSTER_STATE::AWAKE);
    m_stateMachine.RegisterState<ChestMonsterIdleState>(CHEST_MONSTER_STATE::IDLE);
    m_stateMachine.RegisterState<ChestMonsterRunState>(CHEST_MONSTER_STATE::RUN);
    m_stateMachine.RegisterState<ChestMonsterShortAttackState>(CHEST_MONSTER_STATE::SHORT_ATTACK);
    m_stateMachine.RegisterState<ChestMonsterLongAttackState>(CHEST_MONSTER_STATE::LONG_ATTACK);
    m_stateMachine.RegisterState<ChestMonsterHitState>(CHEST_MONSTER_STATE::HIT);
    m_stateMachine.RegisterState<ChestMonsterDeadState>(CHEST_MONSTER_STATE::DEAD);
    m_stateMachine.SetOwner(this);
    m_stateMachine.ChangeState(CHEST_MONSTER_STATE::SLEEP);
}

void ChestMonster::CheckAttackCollision(const AttackInfo& attackInfo)
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

void ChestMonster::Update()
{
    float deltaTime = DELTA_TIME;
    m_stateMachine.Update(deltaTime);
}

void ChestMonster::SetState(CHEST_MONSTER_STATE newState)
{
    m_stateMachine.ChangeState(newState);
}

CHEST_MONSTER_STATE ChestMonster::GetCurrentState() const
{
    return m_stateMachine.GetCurrentState();
}

void ChestMonster::PlayAnimation(CHEST_MONSTER_STATE state)
{
    auto animator = GetAnimator();
    if (animator) {
        animator->Play(static_cast<uint32>(state));
    }
}

shared_ptr<Animator> ChestMonster::GetAnimator()
{
    return GetGameObject()->GetAnimator();
}