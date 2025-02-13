#pragma once
#include "GhostState.h"
class GhostIdleState : public GhostState {
public:
    void Enter(Ghost* ghost) override;
    void Update(Ghost* ghost, float deltaTime) override;
    void Exit(Ghost* ghost) override;

private:
    bool UpdateTarget(Ghost* ghost);
    void UpdateRotation(Ghost* ghost, float deltaTime);
};