#include "pch.h"
#include "GhostAttackState.h"
#include "Ghost.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "GameObject.h"
#include "PlayerManager.h"
#include "Transform.h"

void GhostAttackState::Enter(Ghost* ghost)
{
	ghost->PlayAnimation(GHOST_STATE::ATTACK);
}

void GhostAttackState::Update(Ghost* ghost, float deltaTime)
{
    Player* target = ghost->GetTarget();
    if (!target || !target->GetGameObject()) {
        ghost->SetState(GHOST_STATE::IDLE);
        return;
    }

    // Ÿ���� ���� ������ ������� üũ
    if (!ghost->IsTargetInAttackRange()) {
        ghost->SetState(GHOST_STATE::IDLE);
        return;
    }

    // Ÿ���� ���� �������� ���
    auto targetMovement = target->GetCharacterController();
    if (targetMovement) {
        Vec3 ghostPos = ghost->GetGameObject()->GetTransform()->GetLocalPosition();
        Vec3 targetPos = target->GetGameObject()->GetTransform()->GetLocalPosition();
        Vec3 pullDir = ghostPos - targetPos;
        pullDir.Normalize();

        // �Ÿ��� �ݺ���ϴ� ������ ��� (�������� �� ���ϰ�)
        float distance = ghost->GetDistanceToTarget();
        float pullStrength = std::min(1.0f, ghost->GetAttackRange() / distance);

        targetMovement->Move(pullDir * ghost->GetPullForce() * pullStrength * deltaTime, deltaTime);
    }

    // Ÿ���� ���� ��� ȸ��
    UpdateRotation(ghost, deltaTime);
}

void GhostAttackState::Exit(Ghost* ghost)
{
}

void GhostAttackState::UpdateRotation(Ghost* ghost, float deltaTime)
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
