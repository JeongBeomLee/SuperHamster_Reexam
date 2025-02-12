#include "pch.h"
#include "ChestMonsterDeadState.h"
#include "ChestMonster.h"
#include "CharacterMovement.h"

void ChestMonsterDeadState::Enter(ChestMonster* chestMonster)
{
	chestMonster->PlayAnimation(CHEST_MONSTER_STATE::DEAD);
    m_hasCheckedAnimation = false;

    // 죽을 때 이동 중지
    auto movement = chestMonster->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->StopMovement();
    }
}

void ChestMonsterDeadState::Update(ChestMonster* chestMonster, float deltaTime)
{
    auto animator = chestMonster->GetGameObject()->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        // 사망 애니메이션이 끝나면 게임 오브젝트 비활성화
        chestMonster->GetGameObject()->SetActive(false);

        // 논리적 제거(렉걸리면 진짜 없애주기)
        if (auto controller = chestMonster->GetGameObject()->GetCharacterController()) {
            controller->GetController()->setPosition(PxExtendedVec3(0.f, -9999.f, 0.f));
        }
    }
}

void ChestMonsterDeadState::Exit(ChestMonster* chestMonster)
{
    m_hasCheckedAnimation = false;
}
