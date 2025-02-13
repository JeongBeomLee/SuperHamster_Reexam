#pragma once
#include "GhostState.h"

class Ghost;
class GhostStateMachine {
public:
    void Update(float deltaTime);
    void ChangeState(GHOST_STATE newState);
    GHOST_STATE GetCurrentState() const { return _currentState; }

    template<typename T>
    void RegisterState(GHOST_STATE state) {
        _states[state] = std::make_unique<T>();
    }

private:
    GHOST_STATE _currentState = GHOST_STATE::IDLE;
    std::unordered_map<GHOST_STATE, std::unique_ptr<GhostState>> _states;
    Ghost* _owner = nullptr;

public:
    void SetOwner(Ghost* owner) { _owner = owner; }
};