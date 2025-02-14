#include "pch.h"
#include "BossBreathState.h"
#include "Boss.h"
#include "PlayerManager.h"
#include "EventManager.h"

void BossBreathState::Enter(Boss* boss) {
    boss->PlayAnimation(BOSS_STATE::BREATH);
    m_stateTimer = 0.0f;
    m_attackTimer = 0.0f;
    m_currentAttackIndex = 0;
    m_hasFinalAttack = false;
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

            const auto& positions = boss->GetBreathAttackPositions();
            if (m_currentAttackIndex < positions.size()) {
                PerformBreathAttack(boss, positions[m_currentAttackIndex], false);
                m_currentAttackIndex++;
            }
            else if (m_stateTimer >= PREPARE_TIME + FINAL_ATTACK_DELAY) {
                // ��� ������ ���� ���� ����
                for (const auto& pos : positions) {
                    PerformBreathAttack(boss, pos, true);
                }
                m_hasFinalAttack = true;
                Logger::Instance().Debug("�극�� ���� ���� ����");
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