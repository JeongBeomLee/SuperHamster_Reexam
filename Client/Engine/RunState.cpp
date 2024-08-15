#include "pch.h"
#include "RunState.h"
#include "Player.h"

void RunState::Enter(Player* player)
{
    player->PlayAnimation(PLAYER_STATE::RUN);
}

void RunState::Update(Player* player, float deltaTime)
{
    // Run 상태에서의 추가 로직
}

void RunState::Exit(Player* player)
{
    // Run 상태 종료 시 필요한 로직
}