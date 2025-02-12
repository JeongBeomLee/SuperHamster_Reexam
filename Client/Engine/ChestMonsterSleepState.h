#pragma once
#include "ChestMonsterState.h"
class ChestMonsterSleepState :
    public ChestMonsterState
{
public:
    virtual void Enter(ChestMonster* chestMonster) override;
    virtual void Update(ChestMonster* chestMonster, float deltaTime) override;
    virtual void Exit(ChestMonster* chestMonster) override;

private:
    bool IsPlayerInStage(ChestMonster* chestMonster) const;
    void ResetWakeUpDelay();

private:
    float m_detectionTimer = 0.0f;
    float m_wakeUpDelay = 0.0f;
    bool m_isPlayerDetected = false;
};
