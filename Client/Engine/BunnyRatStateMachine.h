#pragma once
#include "BunnyRatState.h"

class BunnyRat;
class BunnyRatStateMachine {
public:
    void Update(float deltaTime);
    void ChangeState(BUNNY_RAT_STATE newState);
    BUNNY_RAT_STATE GetCurrentState() const { return _currentState; }

    template<typename T>
    void RegisterState(BUNNY_RAT_STATE state) {
        _states[state] = std::make_unique<T>();
    }

private:
    BUNNY_RAT_STATE _currentState = BUNNY_RAT_STATE::IDLE;
    std::unordered_map<BUNNY_RAT_STATE, std::unique_ptr<BunnyRatState>> _states;
    BunnyRat* _owner = nullptr;

public:
    void SetOwner(BunnyRat* owner) { _owner = owner; }
};