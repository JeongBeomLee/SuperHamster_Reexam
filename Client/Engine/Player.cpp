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
    desc.height = 50.0f; // 플레이어 높이
    desc.radius = 25.0f; // 플레이어 반경
    desc.clientID = _playerId;     // 클라이언트 ID 설정
    desc.userData = this;
    desc.material = GEngine->GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f); // 1: 정지 마찰력, 2: 동적 마찰력, 3: 반발 계수
    desc.upDirection = physx::PxVec3(0, 1, 0); // Y축을 위로 설정
    desc.contactOffset = 0.1f; // 플레이어 이동 시 캡슐과의 거리 설정(땅과의 거리)
    desc.climbingMode = physx::PxCapsuleClimbingMode::eEASY; // 플레이어 이동 시 오르기 모드 설정
    desc.slopeLimit = cosf(physx::PxDegToRad(45.f));; // 플레이어 이동 시 올라갈 수 있는 경사 설정
    desc.density = 10.0f; // 플레이어 이동 시 밀도 설정
    desc.stepOffset = 0.5f;  // 플레이어 이동 시 올라갈 수 있는 높이 설정
    desc.maxJumpHeight = 0.0f;  // 플레이어 이동 시 최대 점프 높이 설정 (점프 기능 없으니 0)
    desc.invisibleWallHeight = 0.0f;  // 플레이어 이동 시 통과할 수 없는 벽 높이 설정
    desc.nonWalkableMode = physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING; // 플레이어 이동 시 이동 불가능한 지형 설정
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
    desc.height = 50.0f; // 플레이어 높이
    desc.radius = 25.0f; // 플레이어 반경
    desc.clientID = _playerId;     // 클라이언트 ID 설정
    desc.userData = this;
    desc.material = GEngine->GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f); // 1: 정지 마찰력, 2: 동적 마찰력, 3: 반발 계수
    desc.upDirection = physx::PxVec3(0, 1, 0); // Y축을 위로 설정
    desc.contactOffset = 0.1f; // 플레이어 이동 시 캡슐과의 거리 설정(땅과의 거리)
    desc.climbingMode = physx::PxCapsuleClimbingMode::eEASY; // 플레이어 이동 시 오르기 모드 설정
    desc.slopeLimit = cosf(physx::PxDegToRad(45.f));; // 플레이어 이동 시 올라갈 수 있는 경사 설정
    desc.density = 10.0f; // 플레이어 이동 시 밀도 설정
    desc.stepOffset = 0.5f;  // 플레이어 이동 시 올라갈 수 있는 높이 설정
    desc.maxJumpHeight = 0.0f;  // 플레이어 이동 시 최대 점프 높이 설정 (점프 기능 없으니 0)
    desc.invisibleWallHeight = 0.0f;  // 플레이어 이동 시 통과할 수 없는 벽 높이 설정
    desc.nonWalkableMode = physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING; // 플레이어 이동 시 이동 불가능한 지형 설정
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
