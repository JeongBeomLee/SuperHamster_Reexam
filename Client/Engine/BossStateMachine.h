#pragma once
#include "BossState.h"

class BossStateMachine {
public:
    void Update(float deltaTime);
    void ChangeState(BOSS_STATE newState);
    BOSS_STATE GetCurrentState() const { return _currentState; }

    template<typename T>
    void RegisterState(BOSS_STATE state) {
        _states[state] = std::make_unique<T>();
    }

private:
    BOSS_STATE _currentState = BOSS_STATE::SLEEP;
    std::unordered_map<BOSS_STATE, std::unique_ptr<BossState>> _states;
    Boss* _owner = nullptr;

public:
    void SetOwner(Boss* owner) { _owner = owner; }
};