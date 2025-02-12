#pragma once
#include "ChestMonsterState.h"

class ChestMonster;
class ChestMonsterStateMachine {
public:
    void Update(float deltaTime);
    void ChangeState(CHEST_MONSTER_STATE newState);
    CHEST_MONSTER_STATE GetCurrentState() const { return _currentState; }

    template<typename T>
    void RegisterState(CHEST_MONSTER_STATE state) {
        _states[state] = std::make_unique<T>();
    }

private:
    CHEST_MONSTER_STATE _currentState = CHEST_MONSTER_STATE::SLEEP;
    std::unordered_map<CHEST_MONSTER_STATE, std::unique_ptr<ChestMonsterState>> _states;
    ChestMonster* _owner = nullptr;

public:
    void SetOwner(ChestMonster* owner) { _owner = owner; }
};