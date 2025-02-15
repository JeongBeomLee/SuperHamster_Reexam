#include "pch.h"
#include "BossRightHandAttackState.h"
#include "Boss.h"
#include "Animator.h"
#include "Resources.h"
#include "SoundSystem.h"

void BossRightHandAttackState::Enter(Boss* boss)
{
    boss->PlayAnimation(BOSS_STATE::RIGHT_HAND_ATTACK);
    m_hasCheckedAnimation = false;
    m_hasPerformedAttack = false;

    auto sound = GET_SINGLE(Resources)->Get<Sound>(L"BossSwing");
    if (sound) {
        GET_SINGLE(SoundSystem)->Play3D(sound, boss->GetGameObject()->GetTransform()->GetLocalPosition());
    }

    Logger::Instance().Debug("보스가 오른손 공격 상태로 진입");
}

void BossRightHandAttackState::Update(Boss* boss, float deltaTime)
{
    auto animator = boss->GetGameObject()->GetAnimator();
    if (!animator) return;

    // 공격 판정
    if (!m_hasPerformedAttack && animator->GetCurrentAnimationProgress() >= ATTACK_TIMING) {
        boss->PerformNormalAttack();
        m_hasPerformedAttack = true;
    }

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        boss->SetState(BOSS_STATE::LEFT_HAND_ATTACK);
        Logger::Instance().Debug("오른손 공격 종료, 왼손 공격으로 전환");
    }
}

void BossRightHandAttackState::Exit(Boss* boss)
{
}
