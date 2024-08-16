#include "pch.h"
#include "RollState.h"
#include "Player.h"

void RollState::Enter(Player* player)
{
	player->PlayAnimation(PLAYER_STATE::ROLL);
}

void RollState::Update(Player* player, float deltaTime)
{
	// 돌진 상태에서의 추가 로직이 필요하다면 여기에 구현
}

void RollState::Exit(Player* player)
{
	// 돌진 종료 시 필요한 로직
}
