#include "pch.h"
#include "GhostIdleState.h"
#include "Ghost.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "GameObject.h"
#include "PlayerCameraScript.h"
#include "PlayerManager.h"
#include "Transform.h"

void GhostIdleState::Enter(Ghost* ghost)
{
	ghost->PlayAnimation(GHOST_STATE::IDLE);
}

void GhostIdleState::Update(Ghost* ghost, float deltaTime)
{
    // Stage8 영역 체크 및 타겟 업데이트
    if (!UpdateTarget(ghost)) {
        ghost->SetTarget(nullptr);
        return;
    }

    // 타겟이 있고 공격 범위 안에 있으면 공격 상태로 전환
    if (ghost->GetTarget() && ghost->IsTargetInAttackRange()) {
        ghost->SetState(GHOST_STATE::ATTACK);
        return;
    }

    // 타겟을 향해 회전
    if (ghost->GetTarget()) {
        UpdateRotation(ghost, deltaTime);
    }
}

void GhostIdleState::Exit(Ghost* ghost)
{
}

bool GhostIdleState::UpdateTarget(Ghost* ghost)
{
    auto scene = GET_SINGLE(SceneManager)->GetActiveScene();
    auto mainCamera = scene->GetMainCamera();
    if (!mainCamera) return false;

    auto cameraScript = mainCamera->GetGameObject()->GetMonoBehaviour<PlayerCameraScript>();
    if (!cameraScript || !cameraScript->IsPlayerInArea(L"Stage8"))
        return false;

    const auto& players = GET_SINGLE(PlayerManager)->GetPlayers();
    Player* nearestPlayer = nullptr;
    float minDistance = FLT_MAX;

    Vec3 ghostPos = ghost->GetGameObject()->GetTransform()->GetLocalPosition();

    for (const auto& [playerId, player] : players) {
        if (!player || !player->GetGameObject()) continue;

        Vec3 playerPos = player->GetGameObject()->GetTransform()->GetLocalPosition();
        float distance = (playerPos - ghostPos).Length();

        if (distance < minDistance) {
            minDistance = distance;
            nearestPlayer = player.get();
        }
    }

    ghost->SetTarget(nearestPlayer);
    return nearestPlayer != nullptr;
}

void GhostIdleState::UpdateRotation(Ghost* ghost, float deltaTime)
{
    Player* target = ghost->GetTarget();
    if (!target || !target->GetGameObject()) return;

    Vec3 ghostPos = ghost->GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 targetPos = target->GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 dirToTarget = targetPos - ghostPos;
    dirToTarget.y = 0.0f;
    dirToTarget.Normalize();

    auto movement = ghost->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->SmoothRotation(dirToTarget, deltaTime);
    }
}
