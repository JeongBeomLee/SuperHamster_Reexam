#include "pch.h"
#include "CactusHandAttackState.h"
#include "Cactus.h"
#include "Animator.h"
#include "GameObject.h"

void CactusHandAttackState::Enter(Cactus* cactus)
{
	cactus->PlayAnimation(CACTUS_STATE::HAND_ATTACK);
	m_hasCheckedAnimation = false;
	m_hasPerformedAttack = false;
}

void CactusHandAttackState::Update(Cactus* cactus, float deltaTime)
{
    auto animator = cactus->GetGameObject()->GetAnimator();
    if (!animator) return;

    // ���� ���� Ÿ�̹�
    if (!m_hasPerformedAttack && animator->GetCurrentAnimationProgress() >= 0.5f) {
        cactus->PerformHandAttack();
        m_hasPerformedAttack = true;
    }

    // �ִϸ��̼��� �������� üũ
    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        cactus->SetState(CACTUS_STATE::IDLE);
    }
}

void CactusHandAttackState::Exit(Cactus* cactus)
{
    m_hasCheckedAnimation = false;
	m_hasPerformedAttack = false;
}
