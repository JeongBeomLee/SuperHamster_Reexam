#include "pch.h"
#include "CactusRunState.h"
#include "Cactus.h"
#include "Transform.h"
#include "CharacterMovement.h"

void CactusRunState::Enter(Cactus* cactus)
{
	cactus->PlayAnimation(CACTUS_STATE::RUN);
}

void CactusRunState::Update(Cactus* cactus, float deltaTime)
{
    Player* target = cactus->GetTarget();
    if (!target || !target->GetGameObject()) {
		Logger::Instance().Debug("RUN: 타겟이 없음. IDLE");
        cactus->SetState(CACTUS_STATE::IDLE);
        return;
    }

    // 타겟 방향으로 회전 및 이동
    Vec3 targetPos = target->GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 currentPos = cactus->GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 direction = targetPos - currentPos;
    direction.y = 0.0f;
	direction.Normalize();

    // 캐릭터 이동
    auto movement = cactus->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->SetMoveDirection(direction);
    }

    // 공격 범위에 들어왔는지 체크
    if (cactus->IsTargetInAttackRange()) {
		Logger::Instance().Debug("RUN: 공격 범위에 들어옴. 공격");
        cactus->SetState(rand() % 2 == 0 ? CACTUS_STATE::HAND_ATTACK : CACTUS_STATE::HEAD_ATTACK);
        return;
    }

    // 감지 범위를 벗어났는지 체크
    if (!cactus->IsTargetInDetectionRange()) {
		Logger::Instance().Debug("RUN: 감지 범위를 벗어남. IDLE");
        cactus->SetState(CACTUS_STATE::IDLE);
        return;
    }
}

void CactusRunState::Exit(Cactus* cactus)
{
    auto movement = cactus->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->StopMovement();
    }
}
