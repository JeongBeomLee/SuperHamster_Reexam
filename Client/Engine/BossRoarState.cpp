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

    Logger::Instance().Debug("������ ��ȿ ���·� ����");
}

void BossRoarState::Update(Boss* boss, float deltaTime)
{
    m_roarTimer += deltaTime;

    // ��ȿ ���� �� ������ ��ȯ�� �ʿ����� üũ
    if (!m_phaseTransitionProcessed) {
        if (boss->GetCurrentPhase() == BOSS_PHASE::NONE) {
            // ù ��ȿ �� Phase 1���� ��ȯ
            boss->TransitionToNextPhase();
            m_phaseTransitionProcessed = true;
            Logger::Instance().Debug("���� Phase 1 ����");
        }
        else if (boss->IsPhaseTransitionRequired()) {
            // Phase 2�� ��ȯ
            boss->TransitionToNextPhase();
            m_phaseTransitionProcessed = true;
            Logger::Instance().Debug("���� Phase 2 ����");
        }
    }

    if (m_roarTimer >= ROAR_DURATION) {
        boss->SetState(BOSS_STATE::IDLE);
        // ������ ��ȯ ������ IDLE �������� ǥ��
        if (m_phaseTransitionProcessed) {
            Logger::Instance().Debug("������ ��ȯ �Ϸ�, ���ο� ���� ����");
        }
    }
}

void BossRoarState::Exit(Boss* boss)
{
}
