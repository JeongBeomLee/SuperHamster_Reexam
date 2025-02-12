#include "pch.h"
#include "BunnyRatRunState.h"
#include "BunnyRat.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "GameObject.h"
#include "PlayerCameraScript.h"
#include "PlayerManager.h"
#include "Transform.h"

void BunnyRatRunState::Enter(BunnyRat* bunnyRat)
{
    bunnyRat->PlayAnimation(BUNNY_RAT_STATE::RUN);
}

void BunnyRatRunState::Update(BunnyRat* bunnyRat, float deltaTime)
{
    Player* target = bunnyRat->GetTarget();
    if (!target || !target->GetGameObject()) {
        Logger::Instance().Debug("RUN: 타겟이 없음. IDLE");
        bunnyRat->SetState(BUNNY_RAT_STATE::IDLE);
        return;
    }

    // 타겟 방향으로 회전 및 이동
    Vec3 targetPos = target->GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 currentPos = bunnyRat->GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 direction = targetPos - currentPos;
    direction.y = 0.0f;
    direction.Normalize();

    // 캐릭터 이동
    auto movement = bunnyRat->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->SetMoveDirection(direction);
    }

    // 공격 범위에 들어왔는지 체크
    if (bunnyRat->IsTargetInAttackRange()) {
        Logger::Instance().Debug("RUN: 공격 범위에 들어옴. 공격");
        bunnyRat->SetState(BUNNY_RAT_STATE::ATTACK);
        return;
    }

    // 감지 범위를 벗어났는지 체크
    if (!bunnyRat->IsTargetInDetectionRange()) {
        Logger::Instance().Debug("RUN: 감지 범위를 벗어남. IDLE");
        bunnyRat->SetState(BUNNY_RAT_STATE::IDLE);
        return;
    }
}

void BunnyRatRunState::Exit(BunnyRat* bunnyRat)
{
    auto movement = bunnyRat->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->StopMovement();
    }
}
