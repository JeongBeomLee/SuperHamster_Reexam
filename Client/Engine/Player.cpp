#include "pch.h"
#include "Player.h"
#include "GameObject.h"
#include "Transform.h"
#include "Animator.h"
#include "Engine.h"
#include "IdleState.h"
#include "RunState.h"
#include "AimState.h"
#include "FireState.h"
#include "GetUpState.h"
#include "RollState.h"
#include "HitState.h"
#include "EventManager.h"
#include "Resources.h"
#include "SoundSystem.h"
#include "PlayerHealthBar.h"

Player::Player(uint32_t playerId, std::shared_ptr<GameObject> gameObject) 
    : m_playerId(playerId)
{
    m_gameObject = gameObject;
    CreateComponents();
    InitializeStateMachine();
	RegisterEventHandlers();
}

Player::~Player()
{
	GET_SINGLE(EventManager)->Unsubscribe<Event::PlayerHitEvent>(m_hitEventId);
}

void Player::Update(float deltaTime)
{
    if (!m_gameObject || !m_gameObject->IsActive()) return;

    // 상태 머신 업데이트
    m_stateMachine.Update(deltaTime);

    // 컴포넌트 업데이트는 GameObject 시스템을 통해 자동으로 처리됨
}

void Player::SetState(PLAYER_STATE newState)
{
    if (GetCurrentState() != newState) {
        m_stateMachine.ChangeState(newState);
    }
}

PLAYER_STATE Player::GetCurrentState() const
{
    return m_stateMachine.GetCurrentState();
}

std::shared_ptr<CharacterController> Player::GetCharacterController() const
{
    return m_gameObject->GetCharacterController();
}

std::shared_ptr<CharacterMovement> Player::GetMovementComponent() const
{
	return m_gameObject->GetCharacterMovement();
}

std::shared_ptr<Animator> Player::GetAnimator() const
{
	return m_gameObject->GetAnimator();
}

void Player::PlayAnimation(const PLAYER_STATE state)
{
    auto animator = GetAnimator();
    if (animator) {
        animator->Play(static_cast<uint32>(state));
    }
}

void Player::ProcessNetworkInput(const NetworkInputData& inputData)
{
    if (IsLocalPlayer()) {
        return;  // 로컬 플레이어는 직접 입력을 처리
    }

    // 입력 플래그에 따라 상태 변경
    if (inputData.inputFlags & InputFlags::A) {
        if (GetCurrentState() != PLAYER_STATE::AIM) {
            SetState(PLAYER_STATE::AIM);
            return;
        }
    }

    if (inputData.inputFlags & InputFlags::SPACE) {
        SetState(PLAYER_STATE::ROLL);
        return;
    }

    // 이동 방향 계산
    Vec3 moveDir = Vec3::Zero;
    if (inputData.inputFlags & InputFlags::UP)    moveDir += Vec3(0.f, 0.f, 1.f);
    if (inputData.inputFlags & InputFlags::DOWN)  moveDir += Vec3(0.f, 0.f, -1.f);
    if (inputData.inputFlags & InputFlags::LEFT)  moveDir += Vec3(-1.f, 0.f, 0.f);
    if (inputData.inputFlags & InputFlags::RIGHT) moveDir += Vec3(1.f, 0.f, 0.f);

    if (moveDir != Vec3::Zero) {
        moveDir.Normalize();
        if (GetCurrentState() != PLAYER_STATE::RUN) {
            SetState(PLAYER_STATE::RUN);
        }
        GetMovementComponent()->SetMoveDirection(moveDir);
    }
    else {
        if (GetCurrentState() != PLAYER_STATE::IDLE) {
            SetState(PLAYER_STATE::IDLE);
        }
        GetMovementComponent()->StopMovement();
    }
}

void Player::OnHit(const Event::PlayerHitEvent& event)
{
    // 피격 시 무적 상태가 아니라면 데미지 처리
    if (!m_isInvincible) {
        // 데미지 처리
        m_currentHealth -= event.damage;

		GetGameObject()->GetMonoBehaviour<PlayerHealthBar>()->SetCurrentHealth(m_currentHealth);

        // 피격 상태로 전환
        SetState(PLAYER_STATE::HIT);

		// 피격 사운드 재생
		auto sound = GET_SINGLE(Resources)->Get<Sound>(L"PlayerHit");
		if (sound) {
			GET_SINGLE(SoundSystem)->Play3D(sound, m_gameObject->GetTransform()->GetLocalPosition());
		}

        Logger::Instance().Debug("플레이어가 {}의 데미지를 받음. 남은 체력: {}",
            event.damage, m_currentHealth);
    }
}

void Player::CreateComponents()
{
    // CharacterController 추가
    auto controller = make_shared<CharacterController>();
    m_gameObject->AddComponent(controller);

    // Movement 컴포넌트 추가
    auto movement = make_shared<CharacterMovement>();
    m_gameObject->AddComponent(movement);
}

void Player::InitializeStateMachine()
{
    m_stateMachine.RegisterState<IdleState>(PLAYER_STATE::IDLE);
    m_stateMachine.RegisterState<RunState>(PLAYER_STATE::RUN);
    m_stateMachine.RegisterState<AimState>(PLAYER_STATE::AIM);
    m_stateMachine.RegisterState<FireState>(PLAYER_STATE::FIRE);
    m_stateMachine.RegisterState<GetUpState>(PLAYER_STATE::GETUP);
    m_stateMachine.RegisterState<RollState>(PLAYER_STATE::ROLL);
    m_stateMachine.RegisterState<HitState>(PLAYER_STATE::HIT);
    m_stateMachine.SetOwner(this);

	m_stateMachine.ChangeState(PLAYER_STATE::IDLE);
}

void Player::RegisterEventHandlers()
{
    m_hitEventId = GET_SINGLE(EventManager)->Subscribe<Event::PlayerHitEvent>(
        Event::EventCallback<Event::PlayerHitEvent>(
            [this](const Event::PlayerHitEvent& event) {
                if (event.player == this) {
                    OnHit(event);
                }
            })
    );
}
