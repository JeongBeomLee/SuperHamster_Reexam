#pragma once
#include "PlayerState.h"

enum class PLAYER_STATE
{
    DIE = 0,
    FIRE = 1,
    DANCE1 = 2,
    GETUP = 3,
    DANCE2 = 4,
    AIM = 5,
    HIT = 6,
    RUN = 7,
    ROLL = 8,
    IDLE = 9,
    

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