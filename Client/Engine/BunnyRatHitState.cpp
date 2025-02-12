#include "pch.h"
#include "BunnyRatHitState.h"
#include "BunnyRat.h"
#include "CharacterMovement.h"

void BunnyRatHitState::Enter(BunnyRat* bunnyRat)
{
    bunnyRat->PlayAnimation(BUNNY_RAT_STATE::HIT);
    m_hasCheckedAnimation = false;

    // 피격 시 이동 중지
    auto movement = bunnyRat->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->StopMovement();
    }
}

void BunnyRatHitState::Update(BunnyRat* bunnyRat, float deltaTime)
{
    auto animator = bunnyRat->GetGameObject()->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        bunnyRat->SetState(BUNNY_RAT_STATE::IDLE);
    }
}

void BunnyRatHitState::Exit(BunnyRat* bunnyRat)
{
    m_hasCheckedAnimation = false;
}
