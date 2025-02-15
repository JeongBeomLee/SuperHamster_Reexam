#include "pch.h"
#include "BossRoarState.h"
#include "Boss.h"
#include "Timer.h"
#include "Animator.h"
#include "Resources.h"
#include "SoundSystem.h"

void BossRoarState::Enter(Boss* boss)
{
    boss->PlayAnimation(BOSS_STATE::ROAR);
    m_roarTimer = 0.0f;
    m_phaseTransitionProcessed = false;

    auto sound = GET_SINGLE(Resources)->Get<Sound>(L"BossRoar");
    if (sound) {
        GET_SINGLE(SoundSystem)->Play3D(sound, boss->GetGameObject()->GetTransform()->GetLocalPosition());
    }

    Logger::Instance().Debug("보스가 포효 상태로 진입");
}

void BossRoarState::Update(Boss* boss, float deltaTime)
{
    m_roarTimer += deltaTime;

    // 포효 시작 시 페이즈 전환이 필요한지 체크
    if (!m_phaseTransitionProcessed) {
        if (boss->GetCurrentPhase() == BOSS_PHASE::NONE) {
            // 첫 포효 후 Phase 1으로 전환
            boss->TransitionToNextPhase();
            m_phaseTransitionProcessed = true;
            Logger::Instance().Debug("보스 Phase 1 시작");
        }
        else if (boss->IsPhaseTransitionRequired()) {
            // Phase 2로 전환
            boss->TransitionToNextPhase();
            m_phaseTransitionProcessed = true;
            Logger::Instance().Debug("보스 Phase 2 시작");
        }
    }

    if (m_roarTimer >= ROAR_DURATION) {
        boss->SetState(BOSS_STATE::IDLE);
        // 페이즈 전환 직후의 IDLE 상태임을 표시
        if (m_phaseTransitionProcessed) {
            Logger::Instance().Debug("페이즈 전환 완료, 새로운 패턴 시작");
        }
    }
}

void BossRoarState::Exit(Boss* boss)
{
}
