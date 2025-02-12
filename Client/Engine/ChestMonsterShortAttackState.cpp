#include "pch.h"
#include "ChestMonsterShortAttackState.h"
#include "ChestMonster.h"
#include "Animator.h"
#include "GameObject.h"

void ChestMonsterShortAttackState::Enter(ChestMonster* chestMonster)
{
	chestMonster->PlayAnimation(CHEST_MONSTER_STATE::SHORT_ATTACK);
    m_hasCheckedAnimation = false;
    m_hasPerformedAttack = false;
}

void ChestMonsterShortAttackState::Update(ChestMonster* chestMonster, float deltaTime)
{
    auto animator = chestMonster->GetGameObject()->GetAnimator();
    if (!animator) return;

    // ���� ���� Ÿ�̹�
    if (!m_hasPerformedAttack && animator->GetCurrentAnimationProgress() >= 0.5f) {
        chestMonster->PerformHandAttack();
        m_hasPerformedAttack = true;
    }

    // �ִϸ��̼��� �������� üũ
    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
		chestMonster->SetState(CHEST_MONSTER_STATE::IDLE);
    }
}

void ChestMonsterShortAttackState::Exit(ChestMonster* chestMonster)
{
    m_hasCheckedAnimation = false;
    m_hasPerformedAttack = false;
}
