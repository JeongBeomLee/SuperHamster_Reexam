#include "pch.h"
#include "IdleState.h"
#include "Player.h"

void IdleState::Enter(Player* player)
{
    player->PlayAnimation(PLAYER_STATE::IDLE);
}

void IdleState::Update(Player* player, float deltaTime)
{
    // Idle ���¿����� �߰� ����
}

void IdleState::Exit(Player* player)
{
    // Idle ���� ���� �� �ʿ��� ����
}