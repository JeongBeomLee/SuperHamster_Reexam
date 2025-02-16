#include "pch.h"
#include "AimState.h"
#include "Player.h"
#include "Input.h"

void AimState::Enter(Player* player)
{
	player->PlayAnimation(PLAYER_STATE::AIM);
    player->GetMovementComponent()->StopMovement();
}

void AimState::Update(Player* player, float deltaTime)
{
	// A 키를 떼거나 이동 입력이 없을 때 IDLE 상태로 전환
    if (!INPUT->GetButton(KEY_TYPE::A)) {
        player->SetState(PLAYER_STATE::IDLE);
        return;
    }

    // S 키를 눌렀을 때 FRIE
    if (INPUT->GetButtonDown(KEY_TYPE::S)) {
        player->SetState(PLAYER_STATE::FIRE);
        return;
    }

    // 입력에 따른 조준 방향 계산
    Vec3 aimDir = GetTargetDirection(player);
    // 방향 입력이 있을 경우에만 회전
    if (aimDir != Vec3::Zero) {
        aimDir.Normalize();
        UpdateRotation(player, aimDir, deltaTime);
    }
}

void AimState::Exit(Player* player)
{
}
