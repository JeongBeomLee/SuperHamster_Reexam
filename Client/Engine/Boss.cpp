#include "pch.h"
#include "Boss.h"
#include "BossIdleState.h"
#include "BossRoarReadyState.h"
#include "BossRoarState.h"
#include "BossSleepState.h"
#include "BossAwakeState.h"
#include "BossRunState.h"
#include "BossRightHandAttackState.h"
#include "BossLeftHandAttackState.h"
#include "BossChargeState.h"
#include "BossDownSmashAttackState.h"
#include "BossBreathReadyState.h"
#include "BossBreathState.h"
#include "BossDeadState.h"
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

Boss::Boss()
{
}

Boss::~Boss()
{
	GET_SINGLE(EventManager)->Unsubscribe<Event::ProjectileHitEvent>(m_handHitEventId);
}

void Boss::Awake()
{
	InitializeStateMachine();
	InitializeBreathPositions();
	CreateComponents();
}

void Boss::Start()
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

void Boss::Update()
{
	float deltaTime = DELTA_TIME;
	if (m_currentPhase != BOSS_PHASE::NONE) {
		m_patternTimer += deltaTime;
	}

	m_stateMachine.Update(deltaTime);

	if (GetCurrentState() == BOSS_STATE::IDLE) {
		// �극�� ���� ���� ���� üũ
		if (ShouldStartBreathPattern()) {
			SetState(BOSS_STATE::BREATH_READY);
			m_patternCycleCount = 0;  // ���� ����Ŭ ī��Ʈ ����
		}
		// Phase 1�� ������ ���� ����
		else if (m_normalAttackCount >= 2) {
			if (m_patternTimer >= 1.0f) {
				SetState(BOSS_STATE::CHARGE);
				m_normalAttackCount = 0;
			}
		}
		else if (m_patternTimer >= 1.0f) {
			SetState(BOSS_STATE::RUN);
			m_patternTimer = 0.0f;
		}
	}
}

void Boss::SetState(BOSS_STATE newState)
{
	m_stateMachine.ChangeState(newState);
}

BOSS_STATE Boss::GetCurrentState() const
{
	return m_stateMachine.GetCurrentState();
}

void Boss::PlayAnimation(BOSS_STATE state)
{
	auto animator = GetAnimator();
	if (animator) {
		animator->Play(static_cast<uint32>(state));
	}
}

shared_ptr<Animator> Boss::GetAnimator()
{
	return GetGameObject()->GetAnimator();
}

bool Boss::IsPhaseTransitionRequired() const
{
	return m_currentPhase == BOSS_PHASE::PHASE1 &&
		m_health <= (m_maxHealth * 0.5f);
}

void Boss::TransitionToNextPhase()
{
	switch (m_currentPhase) {
	case BOSS_PHASE::NONE:
		m_currentPhase = BOSS_PHASE::PHASE1;
		break;
	case BOSS_PHASE::PHASE1:
		m_currentPhase = BOSS_PHASE::PHASE2;
		break;
	}

	// ������ ��ȯ �� ���� ī���� �ʱ�ȭ
	m_normalAttackCount = 0;
	m_patternCycleCount = 0;
	m_patternTimer = 0.0f;
}

void Boss::IncrementNormalAttackCount()
{
	m_normalAttackCount++;
	if (m_normalAttackCount >= 2) {
		m_patternTimer = 0.0f;  // ���� ������ ���� Ÿ�̸� �ʱ�ȭ
	}
}

void Boss::IncrementPatternCycleCount()
{
	m_patternCycleCount++;
	m_patternTimer = 0.0f;
}

bool Boss::IsTargetInAttackRange()
{
	return GetDistanceToTarget() <= ATTACK_RANGE;
}

float Boss::GetDistanceToTarget()
{
	if (!m_target || !m_target->GetGameObject() || !GetGameObject()) {
		return FLT_MAX;
	}

	Vec3 targetPos = m_target->GetGameObject()->GetTransform()->GetLocalPosition();
	Vec3 myPos = GetGameObject()->GetTransform()->GetLocalPosition();
	return (targetPos - myPos).Length();
}

void Boss::OnHit(const Event::ProjectileHitEvent& event)
{
	// ������ ó��
	m_health -= ProjectileManager::PROJECTILE_DAMAGE;
	Logger::Instance().Debug("������ ���ݹ���. ���� ü��: {}/{}", m_health, m_maxHealth);

	if (m_health <= 0) {
		SetState(BOSS_STATE::DEAD);
		return;
	}

	// ü���� 50% ���ϰ� �Ǹ� ������ ��ȯ ����
	if (IsPhaseTransitionRequired()) {
		SetState(BOSS_STATE::ROAR_READY);
		Logger::Instance().Debug("���� ü�� 50% ����, ������ ��ȯ ����");
		return;
	}
}

void Boss::CreateComponents()
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
	movement->SetMoveSpeed(RUN_SPEED);
	GetGameObject()->AddComponent(movement);
}

void Boss::InitializeStateMachine()
{
	m_stateMachine.RegisterState<BossSleepState>(BOSS_STATE::SLEEP);
	m_stateMachine.RegisterState<BossAwakeState>(BOSS_STATE::AWAKE);
	m_stateMachine.RegisterState<BossIdleState>(BOSS_STATE::IDLE);
	m_stateMachine.RegisterState<BossRoarReadyState>(BOSS_STATE::ROAR_READY);
	m_stateMachine.RegisterState<BossRoarState>(BOSS_STATE::ROAR);
	m_stateMachine.RegisterState<BossRunState>(BOSS_STATE::RUN);
	m_stateMachine.RegisterState<BossRightHandAttackState>(BOSS_STATE::RIGHT_HAND_ATTACK);
	m_stateMachine.RegisterState<BossLeftHandAttackState>(BOSS_STATE::LEFT_HAND_ATTACK);
	m_stateMachine.RegisterState<BossChargeState>(BOSS_STATE::CHARGE);
	m_stateMachine.RegisterState<BossDownSmashAttackState>(BOSS_STATE::DOWN_SMASH_ATTACK);
	m_stateMachine.RegisterState<BossBreathReadyState>(BOSS_STATE::BREATH_READY);
	m_stateMachine.RegisterState<BossBreathState>(BOSS_STATE::BREATH);
	m_stateMachine.RegisterState<BossDeadState>(BOSS_STATE::DEAD);

	m_stateMachine.SetOwner(this);
	m_stateMachine.ChangeState(BOSS_STATE::SLEEP);
}

void Boss::InitializeBreathPositions()
{
	m_breathAttackPositions.push_back(Vec3(-5.0f, 130.0f, -3760.0f));
	m_breathAttackPositions.push_back(Vec3(-5.0f, 130.0f, -3560.0f));
	m_breathAttackPositions.push_back(Vec3(-5.0f, 130.0f, -3360.0f));
	m_breathAttackPositions.push_back(Vec3(-5.0f, 130.0f, -3160.0f));
}

bool Boss::ShouldStartBreathPattern() const
{
	return m_currentPhase == BOSS_PHASE::PHASE2 &&
		m_patternCycleCount >= 2;  // 2ȸ�� ���� ����Ŭ ��
}
