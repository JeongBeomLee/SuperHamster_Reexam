#include "pch.h"
#include "BossBreathState.h"
#include "Boss.h"
#include "PlayerManager.h"
#include "EventManager.h"
#include "ParticleSystem.h"
#include "Resources.h"
#include "SoundSystem.h"

void BossBreathState::Enter(Boss* boss) {
    boss->PlayAnimation(BOSS_STATE::BREATH);
    m_stateTimer = 0.0f;
    m_attackTimer = 0.0f;
    m_currentAttackIndex = 0;
    m_hasFinalAttack = false;

    auto sound = GET_SINGLE(Resources)->Get<Sound>(L"BossBreath");
    if (sound) {
        GET_SINGLE(SoundSystem)->Play3D(sound, boss->GetGameObject()->GetTransform()->GetLocalPosition());
    }

    Logger::Instance().Debug("������ Breath ���·� ����");
}

void BossBreathState::Update(Boss* boss, float deltaTime) {
    m_stateTimer += deltaTime;

    if (m_stateTimer < PREPARE_TIME) {
        return;  // �غ� �ð� ���
    }

    m_attackTimer += deltaTime;

    if (!m_hasFinalAttack) {
        // ������ ���� ����
        if (m_attackTimer >= ATTACK_INTERVAL) {
            m_attackTimer = 0.0f;

            const auto& breathAreas = boss->GetBreathAreas();
            if (m_currentAttackIndex < breathAreas.size()) {
                const auto& area = breathAreas[m_currentAttackIndex];
                // ���� ������ �߽��� ����Ʈ ���
                int baseIndex = m_currentAttackIndex * 4;
                // �ֺ� 4�� ������ ����Ʈ ���
                for (size_t i = 0; i < area.positions.size(); ++i) {
                    boss->PlayBreathEffect(area.positions[i], baseIndex + i);
                }

                PerformBreathAttack(boss, area.center, false);
                m_currentAttackIndex++;
            }
            else if (m_stateTimer >= PREPARE_TIME + FINAL_ATTACK_DELAY) {
                // ��� ������ ���� ���� ����
                for (size_t areaIdx = 0; areaIdx < breathAreas.size(); ++areaIdx) {
                    const auto& area = breathAreas[areaIdx];

                    // �� ������ �߽����� �ֺ� ����Ʈ ��� ���
                    int baseIndex = areaIdx * 4;
                    for (size_t i = 0; i < area.positions.size(); ++i) {
                        boss->PlayBreathEffect(area.positions[i], baseIndex + i);
                    }

                    PerformBreathAttack(boss, area.center, true);
                }
                m_hasFinalAttack = true;
            }
        }
    }
    else if (m_stateTimer >= PREPARE_TIME + FINAL_ATTACK_DELAY + 1.0f) {
        boss->SetState(BOSS_STATE::IDLE);
        Logger::Instance().Debug("Breath ���� ����, Idle ���·� ��ȯ");
    }
}

void BossBreathState::Exit(Boss* boss)
{
}

void BossBreathState::PerformBreathAttack(Boss* boss, const Vec3& center, bool isFinal) {
    float damage = isFinal ? 40.0f : 20.0f;  // ���� ������ �� ����
    float width = 800.0f;   // x�� ����
    float height = 200.0f;  // z�� ����

    auto sound = GET_SINGLE(Resources)->Get<Sound>(L"BossExplosion");
    if (sound) {
        GET_SINGLE(SoundSystem)->Play3D(sound, center);
    }

    const auto& players = GET_SINGLE(PlayerManager)->GetPlayers();
    for (const auto& [playerId, player] : players) {
        if (!player || !player->GetGameObject()) continue;

        Vec3 playerPos = player->GetGameObject()->GetTransform()->GetWorldPosition();

        // ���� ���� �� �÷��̾� üũ
        if (std::abs(playerPos.x - center.x) <= width * 0.5f &&
            std::abs(playerPos.z - center.z) <= height * 0.5f) {

            Event::PlayerHitEvent event(
                player.get(),
                boss->GetGameObject().get(),
                damage,
                playerPos
            );

            GET_SINGLE(EventManager)->Publish(event);
            Logger::Instance().Debug("�극�� ���� ��Ʈ! �÷��̾� ID: {}, ������: {}",
                playerId, damage);
        }
    }
}