#include "pch.h"
#include "CactusAwakeState.h"
#include "Cactus.h"
#include "Animator.h"

void CactusAwakeState::Enter(Cactus* cactus) 
{
    cactus->PlayAnimation(CACTUS_STATE::AWAKE);
    m_hasCheckedAnimation = false;
}

void CactusAwakeState::Update(Cactus* cactus, float deltaTime) 
{
    auto animator = cactus->GetAnimator();
    if (!animator) return;

    // 애니메이션이 끝났는지 체크
    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        cactus->SetState(CACTUS_STATE::IDLE);
        Logger::Instance().Debug("선인장이 깨어났습니다.");
    }
}

void CactusAwakeState::Exit(Cactus* cactus)
{
}
