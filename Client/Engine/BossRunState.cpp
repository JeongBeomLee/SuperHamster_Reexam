#include "pch.h"
#include "BossRunState.h"
#include "Boss.h"
#include "Animator.h"

void BossRunState::Enter(Boss* boss)
{
	boss->PlayAnimation(BOSS_STATE::RUN);
    auto movement = boss->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->SetMoveSpeed(boss->GetRunSpeed());
    }
	Logger::Instance().Debug("������ Run ���·� ����");
}

void BossRunState::Update(Boss* boss, float deltaTime)
{
    if (Player* target = boss->GetTarget()) {
        Vec3 targetPos = target->GetGameObject()->GetTransform()->GetLocalPosition();
        Vec3 currentPos = boss->GetGameObject()->GetTransform()->GetLocalPosition();
        Vec3 direction = targetPos - currentPos;
        direction.y = 0.0f;
        direction.Normalize();

        // ���� �̵�
        auto movement = boss->GetGameObject()->GetCharacterMovement();
        if (movement) {
            movement->SetMoveDirection(direction);
        }

        // ���� ���� ���� ������ ���� ����
        if (boss->IsTargetInAttackRange()) {
            boss->SetState(BOSS_STATE::RIGHT_HAND_ATTACK);
            return;
        }
    }
    else {
        boss->SetState(BOSS_STATE::IDLE);
    }
}

void BossRunState::Exit(Boss* boss)
{
    auto movement = boss->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->StopMovement();
    }
}