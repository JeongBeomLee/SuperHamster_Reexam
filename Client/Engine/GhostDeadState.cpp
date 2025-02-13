#include "pch.h"
#include "GhostDeadState.h"
#include "Ghost.h"
#include "CharacterMovement.h"

void GhostDeadState::Enter(Ghost* ghost)
{
    ghost->PlayAnimation(GHOST_STATE::DEAD);
    m_hasCheckedAnimation = false;
}

void GhostDeadState::Update(Ghost* ghost, float deltaTime)
{
    auto animator = ghost->GetGameObject()->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        // ��� �ִϸ��̼��� ������ ���� ������Ʈ ��Ȱ��ȭ
        ghost->GetGameObject()->SetActive(false);

        // ���� ����(���ɸ��� ��¥ �����ֱ�)
        if (auto controller = ghost->GetGameObject()->GetCharacterController()) {
            controller->GetController()->setPosition(PxExtendedVec3(0.f, -9999.f, 0.f));
        }
    }
}

void GhostDeadState::Exit(Ghost* ghost)
{
    m_hasCheckedAnimation = false;
}
