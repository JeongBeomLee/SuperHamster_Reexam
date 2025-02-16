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
#include "ParticleSystem.h"
#include "Resources.h"
#include "BossHealthBar.h"

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
	InitializeParticleEffects();
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

void Boss::PerformNormalAttack()
{
	Vec3 attackCenter = GetGameObject()->GetTransform()->GetWorldPosition();
	// �������� �ణ offset�� �־� ���� ��ġ ����
	Vec3 forward = GetGameObject()->GetTransform()->GetLook() * -1.f;
	attackCenter += forward * 100.f;

	AttackInfo attackInfo(attackCenter, ATTACK_RADIUS, ATTACK_DAMAGE, GetGameObject().get());
	const auto& players = GET_SINGLE(PlayerManager)->GetPlayers();

	for (const auto& [playerId, player] : players) {
		if (!player || !player->GetGameObject())
			continue;

		Vec3 playerPos = player->GetGameObject()->GetTransform()->GetWorldPosition();
		float distance = (playerPos - attackInfo.center).Length();

		if (distance <= attackInfo.radius) {
			// �÷��̾ ���� ���� �ȿ� ����
			Logger::Instance().Debug("������ ������ �÷��̾� {}���� ����", playerId);

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
	GetGameObject()->GetMonoBehaviour<BossHealthBar>()->SetCurrentHealth(m_health);
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

void Boss::PlayBreathEffect(const Vec3& position, int index)
{
	if (index >= m_breathEffect.size()) return;

	auto particleSystem = m_breathEffect[index];
	if (!particleSystem) return;

	ParticleSystem::EffectDesc breathDesc;
	breathDesc.type = ParticleSystem::EffectType::BREATH;
	breathDesc.duration = 0.5f;
	breathDesc.startScale = 10.f;
	breathDesc.endScale = 150.f;
	breathDesc.color = Vec4(0.47f, 0.0f, 0.0f, 1.0f);
	breathDesc.material = GET_SINGLE(Resources)->Get<Material>(L"BreathEffect");

	particleSystem->GetTransform()->SetLocalPosition(position);
	particleSystem->Play(breathDesc);
}

void Boss::PlaySmashEffect(const Vec3& footPosition)
{
	ParticleSystem::EffectDesc smashDesc;
	smashDesc.type = ParticleSystem::EffectType::BOSS_SMASH;
	smashDesc.duration = 1.0f;
	smashDesc.startScale = 200.f;
	smashDesc.endScale = 2000.f;
	smashDesc.color = Vec4(0.47f, 0.0f, 0.0f, 1.0f);
	smashDesc.material = GET_SINGLE(Resources)->Get<Material>(L"SmashEffect");

	Vec3 pos = footPosition;
	pos.y += 50.f;
	m_smashEffect->GetTransform()->SetLocalRotation(Vec3(XM_PIDIV2, 0.f, 0.f));
	m_smashEffect->GetTransform()->SetLocalPosition(pos);
	m_smashEffect->Play(smashDesc);
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
	controller->SetRadius(150.f);
	controller->SetHeight(200.f);
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
	// 4���� �ֿ� ���� ����
	vector<Vec3> mainPositions = {
		Vec3(-5.0f, 130.0f, -3760.0f),
		Vec3(-5.0f, 130.0f, -3560.0f),
		Vec3(-5.0f, 130.0f, -3360.0f),
		Vec3(-5.0f, 130.0f, -3160.0f)
	};

	m_breathAreas.clear();
	for (const auto& center : mainPositions) {
		BreathArea area;
		area.center = center;

		// �� �߽����� ���� �ֺ� ��ġ ����
		area.positions = {
			Vec3(center.x + 100.0f, center.y, center.z),
			Vec3(center.x + 300.0f, center.y, center.z),
			Vec3(center.x - 100.0f, center.y, center.z),
			Vec3(center.x - 300.0f, center.y, center.z)
		};

		m_breathAreas.push_back(area);
	}
}

bool Boss::ShouldStartBreathPattern() const
{
	return m_currentPhase == BOSS_PHASE::PHASE2 &&
		m_patternCycleCount >= 2;  // 2ȸ�� ���� ����Ŭ ��
}

void Boss::InitializeParticleEffects()
{
	m_breathEffect.clear();
	
	// �� ��ġ���� ��ƼŬ �ý��� ����
	for (int i = 0; i < 16; ++i) {
		auto particleObj = make_shared<GameObject>();
		particleObj->SetName(L"BreathEffect_" + to_wstring(i));
		particleObj->SetCheckFrustum(false);
		particleObj->AddComponent(make_shared<Transform>());

		auto particleSystem = make_shared<ParticleSystem>(1.f, 1.f);
		particleObj->AddComponent(particleSystem);

		if (auto scene = GET_SINGLE(SceneManager)->GetActiveScene()) {
			scene->AddGameObject(particleObj);
		}
		m_breathEffect.push_back(particleSystem);
	}
	Logger::Instance().Info("���� �극�� ��ƼŬ �ý��� ����");

	// SmashEffect
	{
		auto smashEffectObj = make_shared<GameObject>();
		smashEffectObj->SetName(L"SmashEffect");
		smashEffectObj->SetCheckFrustum(false);
		smashEffectObj->AddComponent(make_shared<Transform>());

		m_smashEffect = make_shared<ParticleSystem>();
		smashEffectObj->AddComponent(m_smashEffect);

		if (auto scene = GET_SINGLE(SceneManager)->GetActiveScene()) {
			scene->AddGameObject(smashEffectObj);
		}
	}
	Logger::Instance().Info("���� ���Ž� ��ƼŬ �ý��� ����");
}
