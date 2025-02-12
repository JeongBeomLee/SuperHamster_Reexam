#include "pch.h"
#include "ChestMonsterStateMachine.h"
#include "ChestMonster.h"

void ChestMonsterStateMachine::Update(float deltaTime)
{
    if (_owner->GetGameObject()->IsActive() == false) return;
    if (_states.count(_currentState) > 0) {
        _states[_currentState]->Update(_owner, deltaTime);
    }
}

void ChestMonsterStateMachine::ChangeState(CHEST_MONSTER_STATE newState)
{
    if (_states.count(_currentState) > 0) {
        _states[_currentState]->Exit(_owner);
    }

    Logger::Instance().Debug("�̹� ���� ����: {} -> {}",
        static_cast<int>(_currentState),
        static_cast<int>(newState));
    _currentState = newState;

    if (_states.count(_currentState) > 0) {
        _states[_currentState]->Enter(_owner);
    }
}