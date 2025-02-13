#pragma once
#include "GhostState.h"
class GhostAttackState : public GhostState {
public:
    void Enter(Ghost* ghost) override;
    void Update(Ghost* ghost, float deltaTime) override;
    void Exit(Ghost* ghost) override;

private:
    void UpdateRotation(Ghost* ghost, float deltaTime);
};