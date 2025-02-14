#include "pch.h"
#include "BossBreathReadyState.h"
#include "Boss.h"

void BossBreathReadyState::Enter(Boss* boss) {
    boss->PlayAnimation(BOSS_STATE::BREATH_READY);
    m_hasCheckedAnimation = false;
    m_hasPositioned = false;

    Logger::Instance().Debug("������ Breath Ready ���·� ����");
}

void BossBreathReadyState::Update(Boss* boss, float deltaTime) {
    if (!m_hasPositioned) {
        // �극�� ���� ��ġ�� �����̵�
        auto controller = boss->GetGameObject()->GetCharacterController();
        if (controller) {
            controller->Teleport(boss->GetBreathPosition());

            // ���� �ٶ󺸵��� ȸ��
            Vec3 forward = Vec3(0.0f, 0.0f, -1.0f);
            boss->GetGameObject()->GetTransform()->LookAt(forward);

            m_hasPositioned = true;
            Logger::Instance().Debug("������ �극�� ��ġ�� �̵�");
        }
    }

    auto animator = boss->GetGameObject()->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        boss->SetState(BOSS_STATE::BREATH);
        Logger::Instance().Debug("Breath Ready ����, Breath ���·� ��ȯ");
    }
}

void BossBreathReadyState::Exit(Boss* boss)
{
}