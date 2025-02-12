#pragma once
#include "BunnyRatState.h"
class BunnyRatRunState :
    public BunnyRatState
{
public:
    virtual void Enter(BunnyRat* bunnyRat) override;
    virtual void Update(BunnyRat* bunnyRat, float deltaTime) override;
    virtual void Exit(BunnyRat* bunnyRat) override;
};

