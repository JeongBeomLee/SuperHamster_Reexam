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

            const auto& breathAreas = boss->GetBreathAreas();
            if (m_currentAttackIndex < breathAreas.size()) {
                const auto& area = breathAreas[m_currentAttackIndex];
                // 현재 영역의 중심점 이펙트 재생
                int baseIndex = m_currentAttackIndex * 4;
                // 주변 4개 지점의 이펙트 재생
                for (size_t i = 0; i < area.positions.size(); ++i) {
                    boss->PlayBreathEffect(area.positions[i], baseIndex + i);
                }

                PerformBreathAttack(boss, area.center, false);
                m_currentAttackIndex++;
            }
            else if (m_stateTimer >= PREPARE_TIME + FINAL_ATTACK_DELAY) {
                // 모든 영역에 대한 최종 공격
                for (size_t areaIdx = 0; areaIdx < breathAreas.size(); ++areaIdx) {
                    const auto& area = breathAreas[areaIdx];

                    // 각 영역의 중심점과 주변 이펙트 모두 재생
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

    auto sound = GET_SINGLE(Resources)->Get<Sound>(L"BossExplosion");
    if (sound) {
        GET_SINGLE(SoundSystem)->Play3D(sound, center);
    }

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