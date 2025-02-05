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

    Vec3 moveDir = GetTargetDirection();
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