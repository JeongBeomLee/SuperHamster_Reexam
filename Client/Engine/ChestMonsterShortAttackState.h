#pragma once
#include "ChestMonsterState.h"
class ChestMonsterShortAttackState :
    public ChestMonsterState
{
public:
    virtual void Enter(ChestMonster* chestMonster) override;
    virtual void Update(ChestMonster* chestMonster, float deltaTime) override;
    virtual void Exit(ChestMonster* chestMonster) override;
private:
    bool m_hasCheckedAnimation = false;
    bool m_hasPerformedAttack = false;
};

