#pragma once
enum class CHEST_MONSTER_STATE {
    SLEEP = 0,
    IDLE = 1,
    SHORT_ATTACK = 3,
    LONG_ATTACK = 4,
    AWAKE = 5,
    RUN = 11,
    HIT = 14,
    DEAD = 16
};

class ChestMonsterState {
public:
    virtual ~ChestMonsterState() = default;
    virtual void Enter(class ChestMonster* chestMonster) = 0;
    virtual void Update(class ChestMonster* chestMonster, float deltaTime) = 0;
    virtual void Exit(class ChestMonster* chestMonster) = 0;
};
