#include "pch.h"
#include "CactusDeadState.h"
#include "Cactus.h"
#include "CharacterMovement.h"

void CactusDeadState::Enter(Cactus* cactus)
{
    cactus->PlayAnimation(CACTUS_STATE::DEAD);
    m_hasCheckedAnimation = false;

    // 죽을 때 이동 중지
    auto movement = cactus->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->StopMovement();
    }
}

void CactusDeadState::Update(Cactus* cactus, float deltaTime)
{
    auto animator = cactus->GetGameObject()->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        // 사망 애니메이션이 끝나면 게임 오브젝트 비활성화
        cactus->GetGameObject()->SetActive(false);

        // 논리적 제거(렉걸리면 진짜 없애주기)
        if (auto controller = cactus->GetGameObject()->GetCharacterController()) {
            controller->GetController()->setPosition(PxExtendedVec3(0.f, -9999.f, 0.f));
        }
    }
}

void CactusDeadState::Exit(Cactus* cactus)
{
    m_hasCheckedAnimation = false;
}
