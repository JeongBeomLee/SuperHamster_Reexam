#pragma once
#include "ChestMonsterState.h"
class ChestMonsterRunState :
    public ChestMonsterState
{
public:
    virtual void Enter(ChestMonster* chestMonster) override;
    virtual void Update(ChestMonster* chestMonster, float deltaTime) override;
    virtual void Exit(ChestMonster* chestMonster) override;
};

