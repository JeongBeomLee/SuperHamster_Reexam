#include "pch.h"
#include "PlayerState.h"
#include "Player.h"
#include "Input.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Scene.h"
#include "Transform.h"

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
		movement->SmoothRotation(targetDirection, deltaTime);
    }
}

bool PlayerState::IsMovementInput() const
{
    return INPUT->GetButton(KEY_TYPE::UP) || INPUT->GetButton(KEY_TYPE::DOWN) ||
        INPUT->GetButton(KEY_TYPE::LEFT) || INPUT->GetButton(KEY_TYPE::RIGHT);
}

Vec3 PlayerState::GetTargetDirection() const
{
    // 카메라 기준 방향 계산
    auto camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
    Vec3 forward = camera->GetTransform()->GetLook();
    Vec3 right = camera->GetTransform()->GetRight();

    // Y축 영향 제거
    forward.y = 0;
    right.y = 0;
    forward.Normalize();
    right.Normalize();

    // 입력에 따른 조준 방향 계산
    Vec3 TargetDir = Vec3::Zero;
    if (INPUT->GetButton(KEY_TYPE::UP))
        TargetDir += forward;
    if (INPUT->GetButton(KEY_TYPE::DOWN))
        TargetDir -= forward;
    if (INPUT->GetButton(KEY_TYPE::LEFT))
        TargetDir -= right;
    if (INPUT->GetButton(KEY_TYPE::RIGHT))
        TargetDir += right;

    return TargetDir;
}
