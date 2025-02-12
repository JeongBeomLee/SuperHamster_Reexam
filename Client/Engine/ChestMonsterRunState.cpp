#include "pch.h"
#include "ChestMonsterRunState.h"
#include "ChestMonster.h"
#include "Transform.h"
#include "CharacterMovement.h"

void ChestMonsterRunState::Enter(ChestMonster* chestMonster)
{
    chestMonster->PlayAnimation(CHEST_MONSTER_STATE::RUN);
}

void ChestMonsterRunState::Update(ChestMonster* chestMonster, float deltaTime)
{
    Player* target = chestMonster->GetTarget();
    if (!target || !target->GetGameObject()) {
        Logger::Instance().Debug("RUN: 타겟이 없음. IDLE");
        chestMonster->SetState(CHEST_MONSTER_STATE::IDLE);
        return;
    }

    // 타겟 방향으로 회전 및 이동
    Vec3 targetPos = target->GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 currentPos = chestMonster->GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 direction = targetPos - currentPos;
    direction.y = 0.0f;
    direction.Normalize();

    // 캐릭터 이동
    auto movement = chestMonster->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->SetMoveDirection(direction);
    }

    // 공격 범위에 들어왔는지 체크
    if (chestMonster->IsTargetInAttackRange()) {
        Logger::Instance().Debug("RUN: 미믹 공격 범위에 들어옴. 공격");
        chestMonster->SetState(rand() % 2 == 0 ? CHEST_MONSTER_STATE::SHORT_ATTACK : CHEST_MONSTER_STATE::LONG_ATTACK);
        return;
    }

    // 감지 범위를 벗어났는지 체크
    if (!chestMonster->IsTargetInDetectionRange()) {
        Logger::Instance().Debug("RUN: 미믹 감지 범위를 벗어남. IDLE");
        chestMonster->SetState(CHEST_MONSTER_STATE::IDLE);
        return;
    }
}

void ChestMonsterRunState::Exit(ChestMonster* chestMonster)
{
    auto movement = chestMonster->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->StopMovement();
    }
}
