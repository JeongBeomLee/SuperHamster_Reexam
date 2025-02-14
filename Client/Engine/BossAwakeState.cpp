#include "pch.h"
#include "BossAwakeState.h"
#include "Boss.h"
#include "Animator.h"

void BossAwakeState::Enter(Boss* boss) {
    boss->PlayAnimation(BOSS_STATE::AWAKE);
    m_hasCheckedAnimation = false;
    Logger::Instance().Debug("������ Awake ���·� ����");
}

void BossAwakeState::Update(Boss* boss, float deltaTime) {
    auto animator = boss->GetGameObject()->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        boss->SetState(BOSS_STATE::IDLE);
        Logger::Instance().Debug("Awake �ִϸ��̼� ����, Idle ���·� ��ȯ");
    }
}

void BossAwakeState::Exit(Boss* boss)
{
}
