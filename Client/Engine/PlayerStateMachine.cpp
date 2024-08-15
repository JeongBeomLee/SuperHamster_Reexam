#include "pch.h"
#include "PlayerStateMachine.h"
#include "Player.h"

void PlayerStateMachine::Update(float deltaTime)
{
    if (_states.count(_currentState) > 0)
    {
        _states[_currentState]->Update(_owner, deltaTime);
    }
}

void PlayerStateMachine::ChangeState(PLAYER_STATE newState)
{
    if (_states.count(_currentState) > 0)
    {
        _states[_currentState]->Exit(_owner);
    }

    _currentState = newState;

    if (_states.count(_currentState) > 0)
    {
        _states[_currentState]->Enter(_owner);
    }
}