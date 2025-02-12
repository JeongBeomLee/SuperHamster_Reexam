#include "pch.h"
#include "ChestMonsterDeadState.h"
#include "ChestMonster.h"
#include "CharacterMovement.h"

void ChestMonsterDeadState::Enter(ChestMonster* chestMonster)
{
	chestMonster->PlayAnimation(CHEST_MONSTER_STATE::DEAD);
    m_hasCheckedAnimation = false;

    // ���� �� �̵� ����
    auto movement = chestMonster->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->StopMovement();
    }
}

void ChestMonsterDeadState::Update(ChestMonster* chestMonster, float deltaTime)
{
    auto animator = chestMonster->GetGameObject()->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        // ��� �ִϸ��̼��� ������ ���� ������Ʈ ��Ȱ��ȭ
        chestMonster->GetGameObject()->SetActive(false);

        // ���� ����(���ɸ��� ��¥ �����ֱ�)
        if (auto controller = chestMonster->GetGameObject()->GetCharacterController()) {
            controller->GetController()->setPosition(PxExtendedVec3(0.f, -9999.f, 0.f));
        }
    }
}

void ChestMonsterDeadState::Exit(ChestMonster* chestMonster)
{
    m_hasCheckedAnimation = false;
}
