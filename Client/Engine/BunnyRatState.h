#pragma once

enum class BUNNY_RAT_STATE {
    IDLE = 0,
    ATTACK = 1,
    DEAD = 4,
    HIT = 6,
    RUN = 16
};

class BunnyRatState {
public:
    virtual ~BunnyRatState() = default;
    virtual void Enter(class BunnyRat* cactus) = 0;
    virtual void Update(class BunnyRat* cactus, float deltaTime) = 0;
    virtual void Exit(class BunnyRat* cactus) = 0;
};