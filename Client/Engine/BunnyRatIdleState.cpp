#include "pch.h"
#include "BunnyRatIdleState.h"
#include "BunnyRat.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "GameObject.h"
#include "PlayerCameraScript.h"
#include "PlayerManager.h"
#include "Transform.h"

void BunnyRatIdleState::Enter(BunnyRat* bunnyRat)
{
    bunnyRat->PlayAnimation(BUNNY_RAT_STATE::IDLE);
}

void BunnyRatIdleState::Update(BunnyRat* bunnyRat, float deltaTime)
{
    if (!UpdateTarget(bunnyRat)) {
        return;
    }

    // Ÿ���� ���� ���� �ȿ� ������ ����
	if (bunnyRat->IsTargetInAttackRange()) {
        Logger::Instance().Debug("IDLE: ���� ������ ����. ����");
        bunnyRat->SetState(BUNNY_RAT_STATE::ATTACK);
        return;
    }

    // Ÿ���� ���� ���� �ȿ� ������ ����
    if (bunnyRat->IsTargetInDetectionRange()) {
        Logger::Instance().Debug("�䳢 ���� ���� �ȿ� ����. �������� ��ȯ");
        bunnyRat->SetState(BUNNY_RAT_STATE::RUN);
        return;
    }
}

void BunnyRatIdleState::Exit(BunnyRat* bunnyRat)
{
}

bool BunnyRatIdleState::UpdateTarget(BunnyRat* bunnyRat)
{
    auto scene = GET_SINGLE(SceneManager)->GetActiveScene();
    auto mainCamera = scene->GetMainCamera();
    if (!mainCamera) return false;

    auto cameraScript = mainCamera->GetGameObject()->GetMonoBehaviour<PlayerCameraScript>();
    if (!cameraScript || !cameraScript->IsPlayerInArea(L"Stage4")) return false;

    const auto& players = GET_SINGLE(PlayerManager)->GetPlayers();
    Player* nearestPlayer = nullptr;
    float minDistance = FLT_MAX;

    Vec3 chestPos = bunnyRat->GetGameObject()->GetTransform()->GetLocalPosition();

    for (const auto& [playerId, player] : players) {
        if (!player || !player->GetGameObject()) continue;

        Vec3 playerPos = player->GetGameObject()->GetTransform()->GetLocalPosition();
        float distance = (playerPos - chestPos).Length();

        if (distance < minDistance) {
            minDistance = distance;
            nearestPlayer = player.get();
        }
    }

    bunnyRat->SetTarget(nearestPlayer);
    return nearestPlayer != nullptr;
}
