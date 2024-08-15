#include "pch.h"
#include "RunState.h"
#include "Player.h"

void RunState::Enter(Player* player)
{
    player->PlayAnimation(PLAYER_STATE::RUN);
}

void RunState::Update(Player* player, float deltaTime)
{
    // Run ���¿����� �߰� ����
}

void RunState::Exit(Player* player)
{
    // Run ���� ���� �� �ʿ��� ����
}