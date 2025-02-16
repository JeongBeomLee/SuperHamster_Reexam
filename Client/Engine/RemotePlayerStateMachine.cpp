#include "pch.h"
#include "NetworkManager.h"
#include "PlayerStateMachine.h"
#include "RemotePlayerStateMachine.h"
#include "GameObject.h"
#include "Player.h"
#include "RemotePlayerState.h"

void RemotePlayerStateMachine::Update(float deltaTime)
{
    if (!_owner || !_owner->GetGameObject()->IsActive()) {
        return;
    }

    if (_states.count(_currentState) > 0) {
        _states[_currentState]->Update(_owner, deltaTime);
    }
}

void RemotePlayerStateMachine::ChangeState(REMOTE_PLAYER_STATE newState)
{
    if (_states.count(_currentState) > 0) {
        _states[_currentState]->Exit(_owner);
    }

    _currentState = newState;

    if (_states.count(_currentState) > 0) {
        _states[_currentState]->Enter(_owner);
    }
}

void RemotePlayerStateMachine::ProcessNetworkInput(const NetworkInputData& inputData)
{
    if (!_owner || !_owner->GetGameObject()->IsActive()) {
        return;
    }

    if (_states.count(_currentState) > 0) {
        _states[_currentState]->ProcessNetworkInput(_owner, inputData);
    }
}
