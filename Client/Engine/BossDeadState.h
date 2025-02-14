#pragma once
#include "BossState.h"
class BossDeadState :
    public BossState
{
public:
    void Enter(Boss* boss) override;;
    void Update(Boss* boss, float deltaTime) override;
    void Exit(Boss* boss) override;

private:
    bool m_hasCheckedAnimation = false;
};