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
                // target�� �� ������ GameObject�� ��쿡�� ó��
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
    // �������� �ణ offset�� �־� ���� ��ġ ����
    Vec3 forward = GetGameObject()->GetTransform()->GetLook() * -1.f;
    attackCenter += forward * 100.f;

    AttackInfo attackInfo(attackCenter, ATTACK_RADIUS, ATTACK_DAMAGE, GetGameObject().get());
    CheckAttackCollision(attackInfo);
}

void BunnyRat::OnHit(const Event::ProjectileHitEvent& event)
{
    // ������ ó��
    m_health -= ProjectileManager::PROJECTILE_DAMAGE;

    Logger::Instance().Debug("�̹��� ����ü�� ����. ���� ü��: {}", m_health);

    if (m_health <= 0) {
        // ü���� 0 ���ϸ� ��� ���·� ��ȯ
        SetState(BUNNY_RAT_STATE::DEAD);
    }
    else {
        // �ƴϸ� �ǰ� ���·� ��ȯ
        SetState(BUNNY_RAT_STATE::HIT);
    }
}

void BunnyRat::CreateComponents()
{
    // CharacterController �߰�
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

    // Movement ������Ʈ �߰�
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
            // �÷��̾ ���� ���� �ȿ� ����
            Logger::Instance().Debug("�̹��� ������ �÷��̾� {}���� ����", playerId);

            // �̺�Ʈ �߻�
            Event::PlayerHitEvent event(
                player.get(),          // ���� �÷��̾�
                attackInfo.attacker,   // ������
                attackInfo.damage,     // ������
                attackInfo.center      // Ÿ�� ��ġ
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
