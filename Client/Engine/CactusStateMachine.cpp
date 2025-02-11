#include "pch.h"
#include "CactusStateMachine.h"
#include "Cactus.h"

void CactusStateMachine::Update(float deltaTime) 
{
	if (_owner->GetGameObject()->IsActive() == false) return;
    if (_states.count(_currentState) > 0) {
        _states[_currentState]->Update(_owner, deltaTime);
    }
}

void CactusStateMachine::ChangeState(CACTUS_STATE newState) 
{
    if (_states.count(_currentState) > 0) {
        _states[_currentState]->Exit(_owner);
        Logger::Instance().Debug("선인장 상태 변경: {} -> {}",
            static_cast<int>(_currentState),
            static_cast<int>(newState));
    }

    _currentState = newState;

    if (_states.count(_currentState) > 0) {
        _states[_currentState]->Enter(_owner);
    }
}