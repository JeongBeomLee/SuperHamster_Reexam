#include "pch.h"
#include "AimState.h"
#include "Player.h"

void AimState::Enter(Player* player)
{
	player->PlayAnimation(PLAYER_STATE::AIM);
}

void AimState::Update(Player* player, float deltaTime)
{
}

void AimState::Exit(Player* player)
{
}
