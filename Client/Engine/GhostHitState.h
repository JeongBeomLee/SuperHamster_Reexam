#pragma once
#include "GhostState.h"
class GhostHitState :
    public GhostState
{
public:
    virtual void Enter(Ghost* ghost) override;
    virtual void Update(Ghost* ghost, float deltaTime) override;
    virtual void Exit(Ghost* ghost) override;
private:
    bool m_hasCheckedAnimation = false;
};

