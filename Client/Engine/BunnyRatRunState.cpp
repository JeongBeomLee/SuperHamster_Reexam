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
        Logger::Instance().Debug("RUN: Ÿ���� ����. IDLE");
        bunnyRat->SetState(BUNNY_RAT_STATE::IDLE);
        return;
    }

    // Ÿ�� �������� ȸ�� �� �̵�
    Vec3 targetPos = target->GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 currentPos = bunnyRat->GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 direction = targetPos - currentPos;
    direction.y = 0.0f;
    direction.Normalize();

    // ĳ���� �̵�
    auto movement = bunnyRat->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->SetMoveDirection(direction);
    }

    // ���� ������ ���Դ��� üũ
    if (bunnyRat->IsTargetInAttackRange()) {
        Logger::Instance().Debug("RUN: ���� ������ ����. ����");
        bunnyRat->SetState(BUNNY_RAT_STATE::ATTACK);
        return;
    }

    // ���� ������ ������� üũ
    if (!bunnyRat->IsTargetInDetectionRange()) {
        Logger::Instance().Debug("RUN: ���� ������ ���. IDLE");
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
