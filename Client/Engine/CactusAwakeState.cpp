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

    // �ִϸ��̼��� �������� üũ
    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        cactus->SetState(CACTUS_STATE::IDLE);
        Logger::Instance().Debug("�������� ������ϴ�.");
    }
}

void CactusAwakeState::Exit(Cactus* cactus)
{
}
