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
    Logger::Instance().Debug("보스가 Breath 상태로 진입");
}

void BossBreathState::Update(Boss* boss, float deltaTime) {
    m_stateTimer += deltaTime;

    if (m_stateTimer < PREPARE_TIME) {
        return;  // 준비 시간 대기
    }

    m_attackTimer += deltaTime;

    if (!m_hasFinalAttack) {
        // 순차적 공격 진행
        if (m_attackTimer >= ATTACK_INTERVAL) {
            m_attackTimer = 0.0f;

            const auto& positions = boss->GetBreathAttackPositions();
            if (m_currentAttackIndex < positions.size()) {
                PerformBreathAttack(boss, positions[m_currentAttackIndex], false);
                m_currentAttackIndex++;
            }
            else if (m_stateTimer >= PREPARE_TIME + FINAL_ATTACK_DELAY) {
                // 모든 영역에 대한 최종 공격
                for (const auto& pos : positions) {
                    PerformBreathAttack(boss, pos, true);
                }
                m_hasFinalAttack = true;
                Logger::Instance().Debug("브레스 최종 공격 실행");
            }
        }
    }
    else if (m_stateTimer >= PREPARE_TIME + FINAL_ATTACK_DELAY + 1.0f) {
        boss->SetState(BOSS_STATE::IDLE);
        Logger::Instance().Debug("Breath 공격 종료, Idle 상태로 전환");
    }
}

void BossBreathState::Exit(Boss* boss)
{
}

void BossBreathState::PerformBreathAttack(Boss* boss, const Vec3& center, bool isFinal) {
    float damage = isFinal ? 40.0f : 20.0f;  // 최종 공격은 더 강력
    float width = 800.0f;   // x축 범위
    float height = 200.0f;  // z축 범위

    const auto& players = GET_SINGLE(PlayerManager)->GetPlayers();
    for (const auto& [playerId, player] : players) {
        if (!player || !player->GetGameObject()) continue;

        Vec3 playerPos = player->GetGameObject()->GetTransform()->GetWorldPosition();

        // 공격 영역 내 플레이어 체크
        if (std::abs(playerPos.x - center.x) <= width * 0.5f &&
            std::abs(playerPos.z - center.z) <= height * 0.5f) {

            Event::PlayerHitEvent event(
                player.get(),
                boss->GetGameObject().get(),
                damage,
                playerPos
            );

            GET_SINGLE(EventManager)->Publish(event);
            Logger::Instance().Debug("브레스 공격 히트! 플레이어 ID: {}, 데미지: {}",
                playerId, damage);
        }
    }
}