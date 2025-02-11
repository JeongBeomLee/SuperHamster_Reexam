#include "pch.h"
#include "CactusHeadAttackState.h"
#include "Cactus.h"
#include "Animator.h"
#include "GameObject.h"

void CactusHeadAttackState::Enter(Cactus* cactus)
{
	cactus->PlayAnimation(CACTUS_STATE::HEAD_ATTACK);
	m_hasCheckedAnimation = false;
}

void CactusHeadAttackState::Update(Cactus* cactus, float deltaTime)
{
    auto animator = cactus->GetGameObject()->GetAnimator();
    if (!animator) return;

    // 애니메이션이 끝났는지 체크
    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        cactus->SetState(CACTUS_STATE::IDLE);
    }
}

void CactusHeadAttackState::Exit(Cactus* cactus)
{
    m_hasCheckedAnimation = false;
}
