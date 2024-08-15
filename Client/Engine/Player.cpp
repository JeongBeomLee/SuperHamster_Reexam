#include "pch.h"
#include "Player.h"
#include "IdleState.h"
#include "RunState.h"
#include "GameObject.h"
#include "Transform.h"
#include "Animator.h"

Player::Player(uint32_t playerId, bool isLocal) : _playerId(playerId), _isLocal(isLocal)
{
    _stateMachine.RegisterState<IdleState>(PLAYER_STATE::IDLE);
    _stateMachine.RegisterState<RunState>(PLAYER_STATE::RUN);
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
    _stateMachine.ChangeState(newState);
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