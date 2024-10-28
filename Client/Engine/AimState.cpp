#include "pch.h"
#include "AimState.h"
#include "Player.h"
#include "Input.h"

void AimState::Enter(Player* player)
{
	player->PlayAnimation(PLAYER_STATE::AIM);
}

void AimState::Update(Player* player, float deltaTime)
{
    // A Ű�� ���� �� IDLE
    if (INPUT->GetButtonUp(KEY_TYPE::A)) {
        player->SetState(PLAYER_STATE::IDLE);
        return;
    }

    // S Ű�� ������ �� FRIE
    if (INPUT->GetButtonDown(KEY_TYPE::S)) {
        player->SetState(PLAYER_STATE::FIRE);
        return;
    }
}

void AimState::Exit(Player* player)
{
}
