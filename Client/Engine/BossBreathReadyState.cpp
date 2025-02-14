#include "pch.h"
#include "BossBreathReadyState.h"
#include "Boss.h"

void BossBreathReadyState::Enter(Boss* boss) {
    boss->PlayAnimation(BOSS_STATE::BREATH_READY);
    m_hasCheckedAnimation = false;
    m_hasPositioned = false;

    Logger::Instance().Debug("보스가 Breath Ready 상태로 진입");
}

void BossBreathReadyState::Update(Boss* boss, float deltaTime) {
    if (!m_hasPositioned) {
        // 브레스 시작 위치로 순간이동
        auto controller = boss->GetGameObject()->GetCharacterController();
        if (controller) {
            controller->Teleport(boss->GetBreathPosition());

            // 정면 바라보도록 회전
            Vec3 forward = Vec3(0.0f, 0.0f, -1.0f);
            boss->GetGameObject()->GetTransform()->LookAt(forward);

            m_hasPositioned = true;
            Logger::Instance().Debug("보스가 브레스 위치로 이동");
        }
    }

    auto animator = boss->GetGameObject()->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        boss->SetState(BOSS_STATE::BREATH);
        Logger::Instance().Debug("Breath Ready 종료, Breath 상태로 전환");
    }
}

void BossBreathReadyState::Exit(Boss* boss)
{
}