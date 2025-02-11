#pragma once
#include "CactusState.h"

class CactusSleepState : public CactusState 
{
public:
    virtual void Enter(Cactus* cactus) override;
    virtual void Update(Cactus* cactus, float deltaTime) override;
    virtual void Exit(Cactus* cactus) override;

private:
    bool IsPlayerInStage(Cactus* cactus) const;
    void ResetWakeUpDelay();

private:
    float m_detectionTimer = 0.0f;
    float m_wakeUpDelay = 0.0f;
    bool m_isPlayerDetected = false;
};