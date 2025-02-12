#include "pch.h"
#include "ChestMonsterLongAttackState.h"
#include "ChestMonster.h"
#include "Animator.h"
#include "GameObject.h"

void ChestMonsterLongAttackState::Enter(ChestMonster* chestMonster)
{
    chestMonster->PlayAnimation(CHEST_MONSTER_STATE::LONG_ATTACK);
    m_hasCheckedAnimation = false;
    m_hasPerformedAttack = false;
}

void ChestMonsterLongAttackState::Update(ChestMonster* chestMonster, float deltaTime)
{
    auto animator = chestMonster->GetGameObject()->GetAnimator();
    if (!animator) return;

    // 공격 판정 타이밍
    if (!m_hasPerformedAttack && animator->GetCurrentAnimationProgress() >= 0.5f) {
        chestMonster->PerformHeadAttack();
        m_hasPerformedAttack = true;
    }

    // 애니메이션이 끝났는지 체크
    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
		chestMonster->SetState(CHEST_MONSTER_STATE::IDLE);
    }
}

void ChestMonsterLongAttackState::Exit(ChestMonster* chestMonster)
{
    m_hasCheckedAnimation = false;
    m_hasPerformedAttack = false;
}
