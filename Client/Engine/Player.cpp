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
    //_stateMachine.RegisterState<IdleState>(PLAYER_STATE::IDLE);
    //_stateMachine.RegisterState<RunState>(PLAYER_STATE::RUN);
    //_stateMachine.RegisterState<AimState>(PLAYER_STATE::AIM);
    //_stateMachine.RegisterState<FireState>(PLAYER_STATE::FIRE);
    //_stateMachine.RegisterState<GetUpState>(PLAYER_STATE::GETUP);
    //_stateMachine.RegisterState<RollState>(PLAYER_STATE::ROLL);
    //_stateMachine.RegisterState<HitState>(PLAYER_STATE::HIT);
    //_stateMachine.SetOwner(this);

    //_gameObject = std::make_shared<GameObject>();
    //_animator = _gameObject->GetAnimator();

    //PLAYER_STATE curState = _stateMachine.GetCurrentState();
    //PlayAnimation(curState);

    //physx::PxCapsuleControllerDesc desc;
    //desc.position = physx::PxExtendedVec3(-460.224, 500, 60.2587);
    //desc.height = 50.0f; // �÷��̾� ����
    //desc.radius = 25.0f; // �÷��̾� �ݰ�
    //desc.clientID = _playerId;     // Ŭ���̾�Ʈ ID ����
    //desc.userData = this;
    //desc.material = GEngine->GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f); // 1: ���� ������, 2: ���� ������, 3: �ݹ� ���
    //desc.upDirection = physx::PxVec3(0, 1, 0); // Y���� ���� ����
    //desc.contactOffset = 0.1f; // �÷��̾� �̵� �� ĸ������ �Ÿ� ����(������ �Ÿ�)
    //desc.climbingMode = physx::PxCapsuleClimbingMode::eEASY; // �÷��̾� �̵� �� ������ ��� ����
    //desc.slopeLimit = cosf(physx::PxDegToRad(45.f));; // �÷��̾� �̵� �� �ö� �� �ִ� ��� ����
    //desc.density = 10.0f; // �÷��̾� �̵� �� �е� ����
    //desc.stepOffset = 0.5f;  // �÷��̾� �̵� �� �ö� �� �ִ� ���� ����
    //desc.maxJumpHeight = 0.0f;  // �÷��̾� �̵� �� �ִ� ���� ���� ���� (���� ��� ������ 0)
    //desc.invisibleWallHeight = 0.0f;  // �÷��̾� �̵� �� ����� �� ���� �� ���� ����
    //desc.nonWalkableMode = physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING; // �÷��̾� �̵� �� �̵� �Ұ����� ���� ����
    //desc.reportCallback = nullptr;
    //desc.behaviorCallback = nullptr;

    //_playerPhysics.controller = GEngine->GetControllerManager()->createController(desc);
    //_playerPhysics.moveDirection = physx::PxVec3(0, 0, 1);

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

std::shared_ptr<PlayerMovement> Player::GetMovementComponent() const
{
	return m_gameObject->GetPlayerMovement();
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
    // CharacterController �߰�
    auto controller = make_shared<CharacterController>();
    m_gameObject->AddComponent(controller);

    // Movement ������Ʈ �߰�
    auto movement = make_shared<PlayerMovement>();
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
}