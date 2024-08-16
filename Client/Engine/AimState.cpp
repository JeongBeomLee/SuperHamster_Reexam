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
    if (INPUT->GetButtonUp(KEY_TYPE::A))
    {
        player->SetState(PLAYER_STATE::IDLE);
        return;
    }
}

void AimState::Exit(Player* player)
{
}
