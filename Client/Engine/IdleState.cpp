#include "pch.h"
#include "IdleState.h"
#include "Player.h"

void IdleState::Enter(Player* player)
{
    player->PlayAnimation(PLAYER_STATE::IDLE);
}

void IdleState::Update(Player* player, float deltaTime)
{
    // Idle 상태에서의 추가 로직
}

void IdleState::Exit(Player* player)
{
    // Idle 상태 종료 시 필요한 로직
}