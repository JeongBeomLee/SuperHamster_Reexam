#include "pch.h"
#include "CactusHitState.h"
#include "Cactus.h"
#include "CharacterMovement.h"

void CactusHitState::Enter(Cactus* cactus)
{
    cactus->PlayAnimation(CACTUS_STATE::HIT);
    m_hasCheckedAnimation = false;

    // 피격 시 이동 중지
    auto movement = cactus->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->StopMovement();
    }
}

void CactusHitState::Update(Cactus* cactus, float deltaTime)
{
    auto animator = cactus->GetGameObject()->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        cactus->SetState(CACTUS_STATE::IDLE);
    }
}

void CactusHitState::Exit(Cactus* cactus)
{
	m_hasCheckedAnimation = false;
}
