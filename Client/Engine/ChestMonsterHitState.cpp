#include "pch.h"
#include "ChestMonsterHitState.h"
#include "ChestMonster.h"
#include "CharacterMovement.h"

void ChestMonsterHitState::Enter(ChestMonster* chestMonster)
{
	chestMonster->PlayAnimation(CHEST_MONSTER_STATE::HIT);
    m_hasCheckedAnimation = false;

    // 피격 시 이동 중지
    auto movement = chestMonster->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->StopMovement();
    }
}

void ChestMonsterHitState::Update(ChestMonster* chestMonster, float deltaTime)
{
    auto animator = chestMonster->GetGameObject()->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
		chestMonster->SetState(CHEST_MONSTER_STATE::IDLE);
    }
}

void ChestMonsterHitState::Exit(ChestMonster* chestMonster)
{
    m_hasCheckedAnimation = false;
}
