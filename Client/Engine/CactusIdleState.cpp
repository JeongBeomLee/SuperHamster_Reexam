#include "pch.h"
#include "CactusIdleState.h"
#include "Cactus.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "GameObject.h"
#include "PlayerCameraScript.h"
#include "PlayerManager.h"
#include "Transform.h"

void CactusIdleState::Enter(Cactus* cactus) 
{
    cactus->PlayAnimation(CACTUS_STATE::IDLE);
}

void CactusIdleState::Update(Cactus* cactus, float deltaTime) 
{
    if (!UpdateTarget(cactus)) {
        cactus->SetState(CACTUS_STATE::SLEEP);
		Logger::Instance().Debug("�÷��̾� ���� -> Sleep");
        return;
    }

    // Ÿ���� ���� ���� �ȿ� ������ ����
    if (cactus->IsTargetInAttackRange()) {
		Logger::Instance().Debug("������ ���� ���� �ȿ� ����. �������� ��ȯ");
        cactus->SetState(rand() % 2 == 0 ? CACTUS_STATE::HAND_ATTACK : CACTUS_STATE::HEAD_ATTACK);
        return;
    }

    // Ÿ���� ���� ���� �ȿ� ������ ����
    if (cactus->IsTargetInDetectionRange()) {
		Logger::Instance().Debug("������ ���� ���� �ȿ� ����. �������� ��ȯ");
        cactus->SetState(CACTUS_STATE::RUN);
        return;
    }
}

void CactusIdleState::Exit(Cactus* cactus) 
{
}

bool CactusIdleState::UpdateTarget(Cactus* cactus)
{
    auto scene = GET_SINGLE(SceneManager)->GetActiveScene();
    auto mainCamera = scene->GetMainCamera();
    if (!mainCamera) return false;

    auto cameraScript = mainCamera->GetGameObject()->GetMonoBehaviour<PlayerCameraScript>();
    if (!cameraScript || !cameraScript->IsPlayerInArea(L"Stage3")) return false;

    const auto& players = GET_SINGLE(PlayerManager)->GetPlayers();
    Player* nearestPlayer = nullptr;
	float minDistance = FLT_MAX;

    Vec3 cactusPos = cactus->GetGameObject()->GetTransform()->GetLocalPosition();

    for (const auto& [playerId, player] : players) {
        if (!player || !player->GetGameObject()) continue;

        Vec3 playerPos = player->GetGameObject()->GetTransform()->GetLocalPosition();
        float distance = (playerPos - cactusPos).Length();

        if (distance < minDistance) {
            minDistance = distance;
            nearestPlayer = player.get();
        }
    }

    cactus->SetTarget(nearestPlayer);
    return nearestPlayer != nullptr;
}
