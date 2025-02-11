#pragma once
#include "PlayerState.h"

enum class PLAYER_STATE
{
    RUN = 0,
    ROLL = 1,
    AIM = 2,
    FIRE = 3,
    HIT = 4,
    GETUP = 5,
    DIE = 6,
    DANCE1 = 7,
    DANCE2 = 8,
    DANCE3 = 9,
    DANCE4 = 10,
    IDLE = 11,

    END
};

class Player;
class PlayerState;
class PlayerStateMachine
{
public:
    void Update(float deltaTime);
    void ChangeState(PLAYER_STATE newState);
    PLAYER_STATE GetCurrentState() const { return _currentState; }

    template<typename T>
    void RegisterState(PLAYER_STATE state) { _states[state] = std::make_unique<T>(); }

private:
    PLAYER_STATE _currentState = PLAYER_STATE::IDLE;
    std::unordered_map<PLAYER_STATE, std::unique_ptr<PlayerState>> _states;
    Player* _owner = nullptr;

public:
    void SetOwner(Player* owner) { _owner = owner; }
};