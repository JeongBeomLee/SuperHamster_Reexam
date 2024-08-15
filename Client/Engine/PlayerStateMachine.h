#pragma once
#include "PlayerState.h"

enum class PLAYER_STATE
{
    FIRE = 3,
    GETUP = 4,
    IDLE = 5,
    HIT = 6,
    RUN = 8,
    ROLL = 9,
    AIM = 11,

    END
};

class Player;
class PlayerStateMachine
{
public:
    void Update(float deltaTime);
    void ChangeState(PLAYER_STATE newState);
    PLAYER_STATE GetCurrentState() const { return _currentState; }

    template<typename T>
    void RegisterState(PLAYER_STATE state)
    {
        _states[state] = std::make_unique<T>();
    }

private:
    PLAYER_STATE _currentState = PLAYER_STATE::IDLE;
    std::unordered_map<PLAYER_STATE, std::unique_ptr<class PlayerState>> _states;
    Player* _owner = nullptr;

public:
    void SetOwner(Player* owner) { _owner = owner; }
};