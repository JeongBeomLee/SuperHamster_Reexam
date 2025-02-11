#include "pch.h"
#include "HitState.h"
#include "Player.h"
#include "Animator.h"

void HitState::Enter(Player* player)
{
    player->PlayAnimation(PLAYER_STATE::HIT);
    m_hasCheckedAnimation = false;

    // 피격 시 이동 중지
    auto movement = player->GetMovementComponent();
    if (movement) {
        movement->StopMovement();
    }

    // 무적 설정
    player->SetInvincible(true);
    Logger::Instance().Debug("플레이어가 피격 상태로 전환");
}

void HitState::Update(Player* player, float deltaTime)
{
    auto animator = player->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        player->SetState(PLAYER_STATE::GETUP);
        Logger::Instance().Debug("피격 애니메이션 종료, 일어나기 상태로 전환");
    }
}

void HitState::Exit(Player* player)
{
    m_hasCheckedAnimation = false;
}
