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
        Logger::Instance().Debug("RUN: Ÿ���� ����. IDLE");
        chestMonster->SetState(CHEST_MONSTER_STATE::IDLE);
        return;
    }

    // Ÿ�� �������� ȸ�� �� �̵�
    Vec3 targetPos = target->GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 currentPos = chestMonster->GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 direction = targetPos - currentPos;
    direction.y = 0.0f;
    direction.Normalize();

    // ĳ���� �̵�
    auto movement = chestMonster->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->SetMoveDirection(direction);
    }

    // ���� ������ ���Դ��� üũ
    if (chestMonster->IsTargetInAttackRange()) {
        Logger::Instance().Debug("RUN: �̹� ���� ������ ����. ����");
        chestMonster->SetState(rand() % 2 == 0 ? CHEST_MONSTER_STATE::SHORT_ATTACK : CHEST_MONSTER_STATE::LONG_ATTACK);
        return;
    }

    // ���� ������ ������� üũ
    if (!chestMonster->IsTargetInDetectionRange()) {
        Logger::Instance().Debug("RUN: �̹� ���� ������ ���. IDLE");
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
