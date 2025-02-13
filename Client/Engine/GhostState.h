#pragma once
enum class GHOST_STATE {
    IDLE = 0,
    ATTACK = 1,
    HIT = 2,
    DEAD = 4
};

class GhostState
{
public:
    virtual ~GhostState() = default;
    virtual void Enter(class Ghost* cactus) = 0;
    virtual void Update(class Ghost* cactus, float deltaTime) = 0;
    virtual void Exit(class Ghost* cactus) = 0;
};

