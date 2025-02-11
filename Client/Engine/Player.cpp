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

    // ���� �ӽ� ������Ʈ
    m_stateMachine.Update(deltaTime);

    // ������Ʈ ������Ʈ�� GameObject �ý����� ���� �ڵ����� ó����
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

void Player::OnHit(const Event::PlayerHitEvent& event)
{
    // �ǰ� �� ���� ���°� �ƴ϶�� ������ ó��
    if (!m_isInvincible) {
        // ������ ó��
        m_health -= event.damage;

        // �ǰ� ���·� ��ȯ
        SetState(PLAYER_STATE::HIT);

        Logger::Instance().Debug("�÷��̾ {}�� �������� ����. ���� ü��: {}",
            event.damage, m_health);
    }
}

void Player::CreateComponents()
{
    // CharacterController �߰�
    auto controller = make_shared<CharacterController>();
    m_gameObject->AddComponent(controller);

    // Movement ������Ʈ �߰�
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
