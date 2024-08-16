#include "pch.h"
#include "FireState.h"
#include "Player.h"

void FireState::Enter(Player* player)
{
	player->PlayAnimation(PLAYER_STATE::FIRE);
}

void FireState::Update(Player* player, float deltaTime)
{
}

void FireState::Exit(Player* player)
{
}
