#include "pch.h"
#include "PlayerState.h"
#include "Player.h"
#include "Input.h"

void PlayerState::UpdateMovement(Player* player, const Vec3& direction, float deltaTime)
{
    auto movement = player->GetMovementComponent();
    if (movement) {
        movement->SetMoveDirection(direction);
    }
}

void PlayerState::UpdateRotation(Player* player, const Vec3& targetDirection, float deltaTime)
{
    auto movement = player->GetMovementComponent();
    if (movement) {
        movement->SetMoveDirection(targetDirection);
    }
}

bool PlayerState::IsMovementInput() const
{
    return INPUT->GetButton(KEY_TYPE::UP) || INPUT->GetButton(KEY_TYPE::DOWN) ||
        INPUT->GetButton(KEY_TYPE::LEFT) || INPUT->GetButton(KEY_TYPE::RIGHT);
}
