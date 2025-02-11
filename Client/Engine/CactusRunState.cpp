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
		Logger::Instance().Debug("RUN: Ÿ���� ����. IDLE");
        cactus->SetState(CACTUS_STATE::IDLE);
        return;
    }

    // Ÿ�� �������� ȸ�� �� �̵�
    Vec3 targetPos = target->GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 currentPos = cactus->GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 direction = targetPos - currentPos;
    direction.y = 0.0f;
	direction.Normalize();

    // ĳ���� �̵�
    auto movement = cactus->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->SetMoveDirection(direction);
    }

    // ���� ������ ���Դ��� üũ
    if (cactus->IsTargetInAttackRange()) {
		Logger::Instance().Debug("RUN: ���� ������ ����. ����");
        cactus->SetState(rand() % 2 == 0 ? CACTUS_STATE::HAND_ATTACK : CACTUS_STATE::HEAD_ATTACK);
        return;
    }

    // ���� ������ ������� üũ
    if (!cactus->IsTargetInDetectionRange()) {
		Logger::Instance().Debug("RUN: ���� ������ ���. IDLE");
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
