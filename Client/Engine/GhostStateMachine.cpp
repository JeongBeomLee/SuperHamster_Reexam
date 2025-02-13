#include "pch.h"
#include "GhostStateMachine.h"
#include "Ghost.h"

void GhostStateMachine::Update(float deltaTime)
{
    if (_owner->GetGameObject()->IsActive() == false) return;
    if (_states.count(_currentState) > 0) {
        _states[_currentState]->Update(_owner, deltaTime);
    }
}

void GhostStateMachine::ChangeState(GHOST_STATE newState)
{
    if (_states.count(_currentState) > 0) {
        _states[_currentState]->Exit(_owner);
    }

    Logger::Instance().Debug("유령 상태 변경: {} -> {}",
        static_cast<int>(_currentState),
        static_cast<int>(newState));
    _currentState = newState;

    if (_states.count(_currentState) > 0) {
        _states[_currentState]->Enter(_owner);
    }
}