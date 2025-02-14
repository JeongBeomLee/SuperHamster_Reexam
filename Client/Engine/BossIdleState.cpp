#include "pch.h"
#include "BossIdleState.h"
#include "Boss.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "GameObject.h"
#include "PlayerCameraScript.h"
#include "PlayerManager.h"
#include "Transform.h"

void BossIdleState::Enter(Boss* boss)
{
    boss->PlayAnimation(BOSS_STATE::IDLE);
    m_stateTimer = 0.0f;

    // 초기 Idle인지 확인 (Awake 직후)
    m_isInitialIdle = (boss->GetCurrentPhase() == BOSS_PHASE::NONE);
    Logger::Instance().Debug("보스가 Idle 상태로 진입 ({})",
        m_isInitialIdle ? "초기 진입" : "일반 진입");
}

void BossIdleState::Update(Boss* boss, float deltaTime)
{
    m_stateTimer += deltaTime;

    if (!UpdateTarget(boss)) {
        boss->SetState(BOSS_STATE::SLEEP);
        return;
    }

    if (m_isInitialIdle && m_stateTimer >= 1.0f) {
        // 초기 Idle에서는 Roar_Ready로 전환
        boss->SetState(BOSS_STATE::ROAR_READY);
        return;
    }
}

void BossIdleState::Exit(Boss* boss)
{
}

bool BossIdleState::UpdateTarget(Boss* boss)
{
    const auto& players = GET_SINGLE(PlayerManager)->GetPlayers();
    Player* nearestPlayer = nullptr;
    float minDistance = FLT_MAX;

    Vec3 bossPos = boss->GetGameObject()->GetTransform()->GetLocalPosition();

    for (const auto& [playerId, player] : players) {
        if (!player || !player->GetGameObject()) continue;

        if (auto cameraScript = GET_SINGLE(SceneManager)->GetActiveScene()
            ->GetMainCamera()->GetGameObject()->GetMonoBehaviour<PlayerCameraScript>()) {

            if (!cameraScript->IsPlayerInArea(L"Stage9")) continue;

            Vec3 playerPos = player->GetGameObject()->GetTransform()->GetLocalPosition();
            float distance = (playerPos - bossPos).Length();

            if (distance < minDistance) {
                minDistance = distance;
                nearestPlayer = player.get();
            }
        }
    }

    boss->SetTarget(nearestPlayer);
    return nearestPlayer != nullptr;
}
