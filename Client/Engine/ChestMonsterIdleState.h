#pragma once
#include "ChestMonsterState.h"
class ChestMonsterIdleState :
    public ChestMonsterState
{
public:
    virtual void Enter(ChestMonster* chestMonster) override;
    virtual void Update(ChestMonster* chestMonster, float deltaTime) override;
    virtual void Exit(ChestMonster* chestMonster) override;

private:
    bool UpdateTarget(ChestMonster* chestMonster);
};

