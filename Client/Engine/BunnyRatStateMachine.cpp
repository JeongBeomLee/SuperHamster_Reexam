#include "pch.h"
#include "BunnyRatStateMachine.h"
#include "BunnyRat.h"

void BunnyRatStateMachine::Update(float deltaTime)
{
    if (_owner->GetGameObject()->IsActive() == false) return;
    if (_states.count(_currentState) > 0) {
        _states[_currentState]->Update(_owner, deltaTime);
    }
}

void BunnyRatStateMachine::ChangeState(BUNNY_RAT_STATE newState)
{
    if (_states.count(_currentState) > 0) {
        _states[_currentState]->Exit(_owner);
    }

    Logger::Instance().Debug("토끼 상태 변경: {} -> {}",
        static_cast<int>(_currentState),
        static_cast<int>(newState));
    _currentState = newState;

    if (_states.count(_currentState) > 0) {
        _states[_currentState]->Enter(_owner);
    }
}