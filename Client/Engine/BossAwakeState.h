#pragma once
#include "BossState.h"
class BossAwakeState :
    public BossState
{
public:
    virtual void Enter(Boss* boss) override;
    virtual void Update(Boss* boss, float deltaTime) override;
    virtual void Exit(Boss* boss) override;

private:
    bool m_hasCheckedAnimation = false;
};

