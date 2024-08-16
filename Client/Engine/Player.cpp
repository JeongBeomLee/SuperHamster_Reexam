#include "pch.h"
#include "Player.h"
#include "GameObject.h"
#include "Transform.h"
#include "Animator.h"
#include "Engine.h"
#include "NetworkManager.h"

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
}

void Player::Update(float deltaTime)
{
    _stateMachine.Update(deltaTime);
}

void Player::SetState(PLAYER_STATE newState)
{
    if (GetCurrentState() != newState) {
        _stateMachine.ChangeState(newState);
        if (IsLocal())
        {
            GEngine->GetNetworkManager()->SendStateUpdate(_playerId, newState);
        }
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