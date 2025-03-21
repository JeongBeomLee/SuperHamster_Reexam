#include "pch.h"
#include "ChestMonsterIdleState.h"
#include "ChestMonster.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "GameObject.h"
#include "PlayerCameraScript.h"
#include "PlayerManager.h"
#include "Transform.h"

void ChestMonsterIdleState::Enter(ChestMonster* chestMonster)
{
    chestMonster->PlayAnimation(CHEST_MONSTER_STATE::IDLE);
}

void ChestMonsterIdleState::Update(ChestMonster* chestMonster, float deltaTime)
{
    if (!UpdateTarget(chestMonster)) {
        chestMonster->SetState(CHEST_MONSTER_STATE::SLEEP);
        Logger::Instance().Debug("플레이어 없음 -> Sleep");
        return;
    }

    // 타겟이 공격 범위 안에 있으면 공격
    if (chestMonster->IsTargetInAttackRange()) {
        Logger::Instance().Debug("IDLE: 공격 범위에 들어옴. 공격");
        chestMonster->SetState(rand() % 2 == 0 ? CHEST_MONSTER_STATE::SHORT_ATTACK : CHEST_MONSTER_STATE::LONG_ATTACK);
        return;
    }

    // 타겟이 감지 범위 안에 있으면 추적
    if (chestMonster->IsTargetInDetectionRange()) {
        Logger::Instance().Debug("미믹 감지 범위 안에 있음. 추적으로 전환");
        chestMonster->SetState(CHEST_MONSTER_STATE::RUN);
        return;
    }
}

void ChestMonsterIdleState::Exit(ChestMonster* chestMonster)
{
}

bool ChestMonsterIdleState::UpdateTarget(ChestMonster* chestMonster)
{
    auto scene = GET_SINGLE(SceneManager)->GetActiveScene();
    auto mainCamera = scene->GetMainCamera();
    if (!mainCamera) return false;

    auto cameraScript = mainCamera->GetGameObject()->GetMonoBehaviour<PlayerCameraScript>();
    if (!cameraScript || !cameraScript->IsPlayerInArea(L"Stage5")) return false;

    const auto& players = GET_SINGLE(PlayerManager)->GetPlayers();
    Player* nearestPlayer = nullptr;
    float minDistance = FLT_MAX;

    Vec3 chestPos = chestMonster->GetGameObject()->GetTransform()->GetLocalPosition();

    for (const auto& [playerId, player] : players) {
        if (!player || !player->GetGameObject()) continue;

        Vec3 playerPos = player->GetGameObject()->GetTransform()->GetLocalPosition();
        float distance = (playerPos - chestPos).Length();

        if (distance < minDistance) {
            minDistance = distance;
            nearestPlayer = player.get();
        }
    }

    chestMonster->SetTarget(nearestPlayer);
    return nearestPlayer != nullptr;
}
