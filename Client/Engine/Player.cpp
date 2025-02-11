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

Player::Player(uint32_t playerId, bool isLocal, std::shared_ptr<GameObject> gameObject) : m_playerId(playerId), m_isLocal(isLocal)
{
    m_gameObject = gameObject;
    CreateComponents();
    InitializeStateMachine();
}

Player::~Player()
{
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