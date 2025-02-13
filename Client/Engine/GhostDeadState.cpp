#include "pch.h"
#include "GhostDeadState.h"
#include "Ghost.h"
#include "CharacterMovement.h"

void GhostDeadState::Enter(Ghost* ghost)
{
    ghost->PlayAnimation(GHOST_STATE::DEAD);
    m_hasCheckedAnimation = false;
}

void GhostDeadState::Update(Ghost* ghost, float deltaTime)
{
    auto animator = ghost->GetGameObject()->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        // 사망 애니메이션이 끝나면 게임 오브젝트 비활성화
        ghost->GetGameObject()->SetActive(false);

        // 논리적 제거(렉걸리면 진짜 없애주기)
        if (auto controller = ghost->GetGameObject()->GetCharacterController()) {
            controller->GetController()->setPosition(PxExtendedVec3(0.f, -9999.f, 0.f));
        }
    }
}

void GhostDeadState::Exit(Ghost* ghost)
{
    m_hasCheckedAnimation = false;
}
