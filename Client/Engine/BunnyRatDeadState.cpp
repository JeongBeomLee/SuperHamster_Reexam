#include "pch.h"
#include "BunnyRatDeadState.h"
#include "BunnyRat.h"
#include "CharacterMovement.h"

void BunnyRatDeadState::Enter(BunnyRat* bunnyRat)
{
    bunnyRat->PlayAnimation(BUNNY_RAT_STATE::DEAD);
    m_hasCheckedAnimation = false;

    // 죽을 때 이동 중지
    auto movement = bunnyRat->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->StopMovement();
    }
}

void BunnyRatDeadState::Update(BunnyRat* bunnyRat, float deltaTime)
{
    auto animator = bunnyRat->GetGameObject()->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        // 사망 애니메이션이 끝나면 게임 오브젝트 비활성화
        bunnyRat->GetGameObject()->SetActive(false);

        // 논리적 제거(렉걸리면 진짜 없애주기)
        if (auto controller = bunnyRat->GetGameObject()->GetCharacterController()) {
            controller->GetController()->setPosition(PxExtendedVec3(0.f, -9999.f, 0.f));
        }
    }
}

void BunnyRatDeadState::Exit(BunnyRat* bunnyRat)
{
    m_hasCheckedAnimation = false;
}
