#include "pch.h"
#include "FireState.h"
#include "Player.h"

void FireState::Enter(Player* player)
{
	player->PlayAnimation(PLAYER_STATE::FIRE);
	m_hasCheckedAnimation = false;
}

void FireState::Update(Player* player, float deltaTime)
{
    auto animator = player->GetAnimator();
    if (!animator)
        return;

    // �ִϸ��̼��� �������� Ȯ��
    if (!m_hasCheckedAnimation && animator->IsAnimationFinished())
    {
        m_hasCheckedAnimation = true;
        player->SetState(PLAYER_STATE::AIM);
        return;
    }
}

void FireState::Exit(Player* player)
{
    m_hasCheckedAnimation = false;
}
