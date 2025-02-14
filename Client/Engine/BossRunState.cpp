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
	Logger::Instance().Debug("보스가 Run 상태로 진입");
}

void BossRunState::Update(Boss* boss, float deltaTime)
{
    if (Player* target = boss->GetTarget()) {
        Vec3 targetPos = target->GetGameObject()->GetTransform()->GetLocalPosition();
        Vec3 currentPos = boss->GetGameObject()->GetTransform()->GetLocalPosition();
        Vec3 direction = targetPos - currentPos;
        direction.y = 0.0f;
        direction.Normalize();

        // 보스 이동
        auto movement = boss->GetGameObject()->GetCharacterMovement();
        if (movement) {
            movement->SetMoveDirection(direction);
        }

        // 공격 범위 내에 들어오면 공격 시작
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