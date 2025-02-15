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
    Logger::Instance().Debug("������ Down Smash Attack ���·� ����");
}

void BossDownSmashAttackState::Update(Boss* boss, float deltaTime)
{
    auto animator = boss->GetGameObject()->GetAnimator();
    if (!animator) return;

    // ���� ����
    if (!m_hasPerformedAttack && animator->GetCurrentAnimationProgress() >= ATTACK_TIMING) {
        Vec3 attackCenter = boss->GetGameObject()->GetTransform()->GetWorldPosition();

        auto pxFootPosition = boss->GetGameObject()->GetCharacterController()->GetController()->getFootPosition();
		Vec3 footPosition = Vec3(pxFootPosition.x, pxFootPosition.y, pxFootPosition.z);
        boss->PlaySmashEffect(footPosition);

        // ���� ���� ��� �÷��̾� üũ
        const auto& players = GET_SINGLE(PlayerManager)->GetPlayers();
        for (const auto& [playerId, player] : players) {
            if (!player || !player->GetGameObject()) continue;

            Vec3 playerPos = player->GetGameObject()->GetTransform()->GetWorldPosition();
            float distance = (playerPos - attackCenter).Length();

            if (distance <= m_attackRadius) {
                // �ǰ� �̺�Ʈ �߻�
                Event::PlayerHitEvent event(
                    player.get(),
                    boss->GetGameObject().get(),
                    m_attackDamage,
                    playerPos
                );

                GET_SINGLE(EventManager)->Publish(event);
                Logger::Instance().Debug("Down Smash Attack ��Ʈ! �÷��̾� ID: {}", playerId);
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
        boss->IncrementPatternCycleCount();  // ���� ����Ŭ ī��Ʈ ����
        Logger::Instance().Debug("Down Smash Attack ����, Idle ���·� ��ȯ");
    }
}

void BossDownSmashAttackState::Exit(Boss* boss)
{
}
