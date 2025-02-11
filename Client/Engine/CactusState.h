#pragma once

enum class CACTUS_STATE {
    SLEEP = 0,
    IDLE = 1,
    HAND_ATTACK = 3,
    AWAKE = 4,
    HEAD_ATTACK = 5,
    RUN = 10,
    HIT = 15,
    DEAD = 17
};

class CactusState {
public:
    virtual ~CactusState() = default;
    virtual void Enter(class Cactus* cactus) = 0;
    virtual void Update(class Cactus* cactus, float deltaTime) = 0;
    virtual void Exit(class Cactus* cactus) = 0;
};