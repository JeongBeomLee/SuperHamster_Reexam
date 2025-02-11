#pragma once
#include "CactusState.h"

class Cactus;
class CactusStateMachine {
public:
    void Update(float deltaTime);
    void ChangeState(CACTUS_STATE newState);
    CACTUS_STATE GetCurrentState() const { return _currentState; }

    template<typename T>
    void RegisterState(CACTUS_STATE state) {
        _states[state] = std::make_unique<T>();
    }

private:
    CACTUS_STATE _currentState = CACTUS_STATE::SLEEP;
    std::unordered_map<CACTUS_STATE, std::unique_ptr<CactusState>> _states;
    Cactus* _owner = nullptr;

public:
    void SetOwner(Cactus* owner) { _owner = owner; }
};