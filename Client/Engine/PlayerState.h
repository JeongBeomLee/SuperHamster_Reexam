#pragma once
#include "PlayerStateMachine.h"

class Player;
class PlayerState
{
public:
    virtual ~PlayerState() = default;

    virtual void Enter(Player* player) = 0;
    virtual void Update(Player* player, float deltaTime) = 0;
    virtual void Exit(Player* player) = 0;

protected:
    // 상태들이 공통적으로 사용할 수 있는 유틸리티 메서드
    void UpdateMovement(Player* player, const Vec3& direction, float deltaTime);
    void UpdateRotation(Player* player, const Vec3& targetDirection, float deltaTime);
    bool IsMovementInput() const;
	Vec3 GetTargetDirection() const;
};