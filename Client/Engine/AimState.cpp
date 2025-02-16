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
	// A Ű�� ���ų� �̵� �Է��� ���� �� IDLE ���·� ��ȯ
    if (!INPUT->GetButton(KEY_TYPE::A)) {
        player->SetState(PLAYER_STATE::IDLE);
        return;
    }

    // S Ű�� ������ �� FRIE
    if (INPUT->GetButtonDown(KEY_TYPE::S)) {
        player->SetState(PLAYER_STATE::FIRE);
        return;
    }

    // �Է¿� ���� ���� ���� ���
    Vec3 aimDir = GetTargetDirection(player);
    // ���� �Է��� ���� ��쿡�� ȸ��
    if (aimDir != Vec3::Zero) {
        aimDir.Normalize();
        UpdateRotation(player, aimDir, deltaTime);
    }
}

void AimState::Exit(Player* player)
{
}
