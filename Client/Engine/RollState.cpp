#include "pch.h"
#include "RollState.h"
#include "Player.h"

void RollState::Enter(Player* player)
{
	player->PlayAnimation(PLAYER_STATE::ROLL);
}

void RollState::Update(Player* player, float deltaTime)
{
	// ���� ���¿����� �߰� ������ �ʿ��ϴٸ� ���⿡ ����
}

void RollState::Exit(Player* player)
{
	// ���� ���� �� �ʿ��� ����
}
