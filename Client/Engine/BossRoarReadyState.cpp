#include "pch.h"
#include "BossRoarReadyState.h"
#include "Boss.h"
#include "Animator.h"

void BossRoarReadyState::Enter(Boss* boss)
{
    boss->PlayAnimation(BOSS_STATE::ROAR_READY);
    m_hasCheckedAnimation = false;

    Logger::Instance().Debug("보스가 포효 준비 상태로 진입");
}

void BossRoarReadyState::Update(Boss* boss, float deltaTime)
{
    auto animator = boss->GetGameObject()->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        boss->SetState(BOSS_STATE::ROAR);
        Logger::Instance().Debug("포효 준비 완료, Roar 상태로 전환");
    }
}

void BossRoarReadyState::Exit(Boss* boss)
{
}
