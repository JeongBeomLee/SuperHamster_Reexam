#pragma once
#include "BunnyRatState.h"
class BunnyRatIdleState :
    public BunnyRatState
{
public:
    virtual void Enter(BunnyRat* bunnyRat) override;
    virtual void Update(BunnyRat* bunnyRat, float deltaTime) override;
    virtual void Exit(BunnyRat* bunnyRat) override;

private:
    bool UpdateTarget(BunnyRat* bunnyRat);
};

