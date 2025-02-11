#include "pch.h"
#include "HitState.h"
#include "Player.h"
#include "Animator.h"

void HitState::Enter(Player* player)
{
    player->PlayAnimation(PLAYER_STATE::HIT);
    m_hasCheckedAnimation = false;

    // �ǰ� �� �̵� ����
    auto movement = player->GetMovementComponent();
    if (movement) {
        movement->StopMovement();
    }

    // ���� ����
    player->SetInvincible(true);
    Logger::Instance().Debug("�÷��̾ �ǰ� ���·� ��ȯ");
}

void HitState::Update(Player* player, float deltaTime)
{
    auto animator = player->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        player->SetState(PLAYER_STATE::GETUP);
        Logger::Instance().Debug("�ǰ� �ִϸ��̼� ����, �Ͼ�� ���·� ��ȯ");
    }
}

void HitState::Exit(Player* player)
{
    m_hasCheckedAnimation = false;
}
