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
    // ���µ��� ���������� ����� �� �ִ� ��ƿ��Ƽ �޼���
    void UpdateMovement(Player* player, const Vec3& direction, float deltaTime);
    void UpdateRotation(Player* player, const Vec3& targetDirection, float deltaTime);
    bool IsMovementInput() const;
	Vec3 GetTargetDirection() const;
};