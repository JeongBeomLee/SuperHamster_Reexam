#pragma once

enum class BOSS_STATE {
    RIGHT_HAND_ATTACK = 12,
    DOWN_SMASH_ATTACK = 14,
    LEFT_HAND_ATTACK  = 16,
    DEAD  = 25,
    AWAKE = 32,
    SLEEP = 35,
    CHARGE = 56,
    RUN = 58,
    BREATH = 71,
    BREATH_READY = 73,
    IDLE = 74,
    ROAR = 77,
    ROAR_READY = 79
};

enum class BOSS_PHASE {
    NONE,
    PHASE1,
    PHASE2
};

class BossState
{
public:
    virtual ~BossState() = default;
    virtual void Enter(class Boss* cactus) = 0;
    virtual void Update(class Boss* cactus, float deltaTime) = 0;
    virtual void Exit(class Boss* cactus) = 0;
};

