#pragma once
#include "CactusState.h"
class CactusDeadState : public CactusState
{
public:
    void Enter(Cactus* cactus) override;;
    void Update(Cactus* cactus, float deltaTime) override;
    void Exit(Cactus* cactus) override;

private:
    bool m_hasCheckedAnimation = false;
};

