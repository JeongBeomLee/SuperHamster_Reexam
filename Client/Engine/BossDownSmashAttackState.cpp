#include "pch.h"
#include "BossDownSmashAttackState.h"
#include "Boss.h"
#include "Player.h"
#include "PlayerManager.h"
#include "Animator.h"
#include "EventManager.h"
#include "Resources.h"
#include "SoundSystem.h"

void BossDownSmashAttackState::Enter(Boss* boss)
{
    boss->PlayAnimation(BOSS_STATE::DOWN_SMASH_ATTACK);
    m_hasCheckedAnimation = false;
    m_hasPerformedAttack = false;
    Logger::Instance().Debug("보스가 Down Smash Attack 상태로 진입");
}

void BossDownSmashAttackState::Update(Boss* boss, float deltaTime)
{
    auto animator = boss->GetGameObject()->GetAnimator();
    if (!animator) return;

    // 공격 판정
    if (!m_hasPerformedAttack && animator->GetCurrentAnimationProgress() >= ATTACK_TIMING) {
        Vec3 attackCenter = boss->GetGameObject()->GetTransform()->GetWorldPosition();

        auto pxFootPosition = boss->GetGameObject()->GetCharacterController()->GetController()->getFootPosition();
		Vec3 footPosition = Vec3(pxFootPosition.x, pxFootPosition.y, pxFootPosition.z);
        boss->PlaySmashEffect(footPosition);

        // 범위 내의 모든 플레이어 체크
        const auto& players = GET_SINGLE(PlayerManager)->GetPlayers();
        for (const auto& [playerId, player] : players) {
            if (!player || !player->GetGameObject()) continue;

            Vec3 playerPos = player->GetGameObject()->GetTransform()->GetWorldPosition();
            float distance = (playerPos - attackCenter).Length();

            if (distance <= m_attackRadius) {
                // 피격 이벤트 발생
                Event::PlayerHitEvent event(
                    player.get(),
                    boss->GetGameObject().get(),
                    m_attackDamage,
                    playerPos
                );

                GET_SINGLE(EventManager)->Publish(event);
                Logger::Instance().Debug("Down Smash Attack 히트! 플레이어 ID: {}", playerId);
            }
        }

        auto sound = GET_SINGLE(Resources)->Get<Sound>(L"BossSmash");
        if (sound) {
            GET_SINGLE(SoundSystem)->Play3D(sound, boss->GetGameObject()->GetTransform()->GetLocalPosition());
        }
        m_hasPerformedAttack = true;
    }

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        boss->SetState(BOSS_STATE::IDLE);
        boss->IncrementPatternCycleCount();  // 패턴 사이클 카운트 증가
        Logger::Instance().Debug("Down Smash Attack 종료, Idle 상태로 전환");
    }
}

void BossDownSmashAttackState::Exit(Boss* boss)
{
}
