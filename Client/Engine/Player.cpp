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

Player::Player(uint32_t playerId, bool isLocal) : _playerId(playerId), _isLocal(isLocal)
{
    _stateMachine.RegisterState<IdleState>(PLAYER_STATE::IDLE);
    _stateMachine.RegisterState<RunState>(PLAYER_STATE::RUN);
    _stateMachine.RegisterState<AimState>(PLAYER_STATE::AIM);
    _stateMachine.RegisterState<FireState>(PLAYER_STATE::FIRE);
    _stateMachine.RegisterState<GetUpState>(PLAYER_STATE::GETUP);
    _stateMachine.RegisterState<RollState>(PLAYER_STATE::ROLL);
    _stateMachine.RegisterState<HitState>(PLAYER_STATE::HIT);
    _stateMachine.SetOwner(this);

    _gameObject = std::make_shared<GameObject>();
    _animator = _gameObject->GetAnimator();

    PLAYER_STATE curState = _stateMachine.GetCurrentState();
    PlayAnimation(curState);

    physx::PxCapsuleControllerDesc desc;
    desc.position = physx::PxExtendedVec3(-460.224, 500, 60.2587);
    desc.height = 50.0f; // �÷��̾� ����
    desc.radius = 25.0f; // �÷��̾� �ݰ�
    desc.clientID = _playerId;     // Ŭ���̾�Ʈ ID ����
    desc.userData = this;
    desc.material = GEngine->GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f); // 1: ���� ������, 2: ���� ������, 3: �ݹ� ���
    desc.upDirection = physx::PxVec3(0, 1, 0); // Y���� ���� ����
    desc.contactOffset = 0.1f; // �÷��̾� �̵� �� ĸ������ �Ÿ� ����(������ �Ÿ�)
    desc.climbingMode = physx::PxCapsuleClimbingMode::eEASY; // �÷��̾� �̵� �� ������ ��� ����
    desc.slopeLimit = cosf(physx::PxDegToRad(45.f));; // �÷��̾� �̵� �� �ö� �� �ִ� ��� ����
    desc.density = 10.0f; // �÷��̾� �̵� �� �е� ����
    desc.stepOffset = 0.5f;  // �÷��̾� �̵� �� �ö� �� �ִ� ���� ����
    desc.maxJumpHeight = 0.0f;  // �÷��̾� �̵� �� �ִ� ���� ���� ���� (���� ��� ������ 0)
    desc.invisibleWallHeight = 0.0f;  // �÷��̾� �̵� �� ����� �� ���� �� ���� ����
    desc.nonWalkableMode = physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING; // �÷��̾� �̵� �� �̵� �Ұ����� ���� ����
    desc.reportCallback = nullptr;
    desc.behaviorCallback = nullptr;

    _playerPhysics.controller = GEngine->GetControllerManager()->createController(desc);
    _playerPhysics.moveDirection = physx::PxVec3(0, 0, 1);
}

Player::Player(uint32_t playerId, bool isLocal, std::shared_ptr<GameObject> gameObject) : _playerId(playerId), _isLocal(isLocal), _gameObject(gameObject)
{
    _stateMachine.RegisterState<IdleState>(PLAYER_STATE::IDLE);
    _stateMachine.RegisterState<RunState>(PLAYER_STATE::RUN);
    _stateMachine.RegisterState<AimState>(PLAYER_STATE::AIM);
    _stateMachine.RegisterState<FireState>(PLAYER_STATE::FIRE);
    _stateMachine.RegisterState<GetUpState>(PLAYER_STATE::GETUP);
    _stateMachine.RegisterState<RollState>(PLAYER_STATE::ROLL);
    _stateMachine.RegisterState<HitState>(PLAYER_STATE::HIT);
    _stateMachine.SetOwner(this);

    _animator = _gameObject->GetAnimator();

    PLAYER_STATE curState = _stateMachine.GetCurrentState();
    PlayAnimation(curState);

    physx::PxCapsuleControllerDesc desc;
    desc.position = physx::PxExtendedVec3(-460.224, 500, 60.2587);
    desc.height = 50.0f; // �÷��̾� ����
    desc.radius = 25.0f; // �÷��̾� �ݰ�
    desc.clientID = _playerId;     // Ŭ���̾�Ʈ ID ����
    desc.userData = this;
    desc.material = GEngine->GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f); // 1: ���� ������, 2: ���� ������, 3: �ݹ� ���
    desc.upDirection = physx::PxVec3(0, 1, 0); // Y���� ���� ����
    desc.contactOffset = 0.1f; // �÷��̾� �̵� �� ĸ������ �Ÿ� ����(������ �Ÿ�)
    desc.climbingMode = physx::PxCapsuleClimbingMode::eEASY; // �÷��̾� �̵� �� ������ ��� ����
    desc.slopeLimit = cosf(physx::PxDegToRad(45.f));; // �÷��̾� �̵� �� �ö� �� �ִ� ��� ����
    desc.density = 10.0f; // �÷��̾� �̵� �� �е� ����
    desc.stepOffset = 0.5f;  // �÷��̾� �̵� �� �ö� �� �ִ� ���� ����
    desc.maxJumpHeight = 0.0f;  // �÷��̾� �̵� �� �ִ� ���� ���� ���� (���� ��� ������ 0)
    desc.invisibleWallHeight = 0.0f;  // �÷��̾� �̵� �� ����� �� ���� �� ���� ����
    desc.nonWalkableMode = physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING; // �÷��̾� �̵� �� �̵� �Ұ����� ���� ����
    desc.reportCallback = nullptr;
    desc.behaviorCallback = nullptr;

    _playerPhysics.controller = GEngine->GetControllerManager()->createController(desc);
    _playerPhysics.moveDirection = physx::PxVec3(0, 0, 1);
}

void Player::Update(float deltaTime)
{
    _stateMachine.Update(deltaTime);
    ApplyGravity(deltaTime);
}

void Player::SetState(PLAYER_STATE newState)
{
    if (GetCurrentState() != newState) 
    {
        _stateMachine.ChangeState(newState);
    }
}

void Player::SetPosition(const Vec3& position)
{
    _gameObject->GetTransform()->SetLocalPosition(position);
}

void Player::SetRotation(const Vec3& rotation)
{
    _gameObject->GetTransform()->SetLocalRotation(rotation);
}

void Player::PlayAnimation(const PLAYER_STATE state)
{
    if (_animator)
    {
		_animator->Play(static_cast<uint32>(state));
    }
}

void Player::ApplyGravity(float deltaTime)
{
    if(_playerPhysics.controller)
	{
        physx::PxVec3 gravity(0, -9.81f * deltaTime, 0);
        _playerPhysics.controller->move(physx::PxVec3(0, -9.81f, 0), 0.0001f, deltaTime, physx::PxControllerFilters());
	}
}
