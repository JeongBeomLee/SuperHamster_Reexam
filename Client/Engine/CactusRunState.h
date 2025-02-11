#pragma once
#include "CactusState.h"

class CactusRunState : public CactusState
{
    void Enter(Cactus* cactus) override;
    void Update(Cactus* cactus, float deltaTime) override;
    void Exit(Cactus* cactus) override;
};