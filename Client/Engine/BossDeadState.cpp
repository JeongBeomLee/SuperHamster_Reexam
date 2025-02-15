#include "pch.h"
#include "BossDeadState.h"
#include "Boss.h"
#include "CharacterMovement.h"
#include "Resources.h"
#include "SoundSystem.h"

void BossDeadState::Enter(Boss* boss)
{
    boss->PlayAnimation(BOSS_STATE::DEAD);
    m_hasCheckedAnimation = false;

    // 죽을 때 이동 중지
    auto movement = boss->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->StopMovement();
    }

    auto sound = GET_SINGLE(Resources)->Get<Sound>(L"BossDead");
    if (sound) {
        GET_SINGLE(SoundSystem)->Play3D(sound, boss->GetGameObject()->GetTransform()->GetLocalPosition());
    }
}

void BossDeadState::Update(Boss* boss, float deltaTime)
{
    auto animator = boss->GetGameObject()->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        // 사망 애니메이션이 끝나면 게임 오브젝트 비활성화
        boss->GetGameObject()->SetActive(false);

        // 논리적 제거(렉걸리면 진짜 없애주기)
        if (auto controller = boss->GetGameObject()->GetCharacterController()) {
            controller->GetController()->setPosition(PxExtendedVec3(0.f, -9999.f, 0.f));
        }
    }
}

void BossDeadState::Exit(Boss* boss)
{
    m_hasCheckedAnimation = false;
}
