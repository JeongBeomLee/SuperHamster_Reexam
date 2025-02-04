#include "pch.h"
#include "RunState.h"
#include "Player.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "Transform.h"
#include "Input.h"

void RunState::Enter(Player* player)
{
    player->PlayAnimation(PLAYER_STATE::RUN);
}

void RunState::Update(Player* player, float deltaTime)
{
    if (!IsMovementInput()) {
        player->SetState(PLAYER_STATE::IDLE);
        return;
    }

    // 카메라 기준 이동 방향 계산
    Vec3 moveDir = Vec3::Zero;
    auto camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
    Vec3 forward = camera->GetTransform()->GetLook();
    Vec3 right = camera->GetTransform()->GetRight();

    // Y축 영향 제거
    forward.y = 0;
    right.y = 0;
    forward.Normalize();
    right.Normalize();

    if (INPUT->GetButton(KEY_TYPE::UP))
        moveDir += forward;
    if (INPUT->GetButton(KEY_TYPE::DOWN))
        moveDir -= forward;
    if (INPUT->GetButton(KEY_TYPE::LEFT))
        moveDir -= right;
    if (INPUT->GetButton(KEY_TYPE::RIGHT))
        moveDir += right;

    if (moveDir != Vec3::Zero) {
        moveDir.Normalize();
        UpdateMovement(player, moveDir, deltaTime);
    }

    if (INPUT->GetButton(KEY_TYPE::A)) {
        player->SetState(PLAYER_STATE::AIM);
        return;
    }

    if (INPUT->GetButtonDown(KEY_TYPE::SPACE)) {
        player->SetState(PLAYER_STATE::ROLL);
        return;
    }
}

void RunState::Exit(Player* player)
{
    // Run 상태 종료 시 필요한 로직
}