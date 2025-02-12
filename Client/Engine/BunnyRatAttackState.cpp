#include "pch.h"
#include "BunnyRatAttackState.h"
#include "BunnyRat.h"
#include "Animator.h"
#include "GameObject.h"

void BunnyRatAttackState::Enter(BunnyRat* bunnyRat)
{
    bunnyRat->PlayAnimation(BUNNY_RAT_STATE::ATTACK);
    m_hasCheckedAnimation = false;
    m_hasPerformedAttack = false;
}

void BunnyRatAttackState::Update(BunnyRat* bunnyRat, float deltaTime)
{
    auto animator = bunnyRat->GetGameObject()->GetAnimator();
    if (!animator) return;

    // 공격 판정 타이밍
    if (!m_hasPerformedAttack && animator->GetCurrentAnimationProgress() >= 0.5f) {
        bunnyRat->PerformAttack();
        m_hasPerformedAttack = true;
    }

    // 애니메이션이 끝났는지 체크
    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        bunnyRat->SetState(BUNNY_RAT_STATE::IDLE);
    }
}

void BunnyRatAttackState::Exit(BunnyRat* bunnyRat)
{
    m_hasCheckedAnimation = false;
    m_hasPerformedAttack = false;
}
