#include "pch.h"
#include "GhostHitState.h"
#include "Ghost.h"
#include "CharacterMovement.h"

void GhostHitState::Enter(Ghost* ghost)
{
    ghost->PlayAnimation(GHOST_STATE::HIT);
    m_hasCheckedAnimation = false;
}

void GhostHitState::Update(Ghost* ghost, float deltaTime)
{
    auto animator = ghost->GetGameObject()->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        ghost->SetState(GHOST_STATE::IDLE);
    }
}

void GhostHitState::Exit(Ghost* ghost)
{
    m_hasCheckedAnimation = false;
}
