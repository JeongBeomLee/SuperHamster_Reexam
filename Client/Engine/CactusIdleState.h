#pragma once
#include "CactusState.h"

class CactusIdleState : public CactusState 
{
public:
    virtual void Enter(Cactus* cactus) override;
    virtual void Update(Cactus* cactus, float deltaTime) override;
    virtual void Exit(Cactus* cactus) override;

private:
    bool UpdateTarget(Cactus* cactus);
};
