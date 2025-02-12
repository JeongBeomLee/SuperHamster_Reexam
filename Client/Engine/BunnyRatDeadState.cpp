#include "pch.h"
#include "BunnyRatDeadState.h"
#include "BunnyRat.h"
#include "CharacterMovement.h"

void BunnyRatDeadState::Enter(BunnyRat* bunnyRat)
{
    bunnyRat->PlayAnimation(BUNNY_RAT_STATE::DEAD);
    m_hasCheckedAnimation = false;

    // ���� �� �̵� ����
    auto movement = bunnyRat->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->StopMovement();
    }
}

void BunnyRatDeadState::Update(BunnyRat* bunnyRat, float deltaTime)
{
    auto animator = bunnyRat->GetGameObject()->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        // ��� �ִϸ��̼��� ������ ���� ������Ʈ ��Ȱ��ȭ
        bunnyRat->GetGameObject()->SetActive(false);

        // ���� ����(���ɸ��� ��¥ �����ֱ�)
        if (auto controller = bunnyRat->GetGameObject()->GetCharacterController()) {
            controller->GetController()->setPosition(PxExtendedVec3(0.f, -9999.f, 0.f));
        }
    }
}

void BunnyRatDeadState::Exit(BunnyRat* bunnyRat)
{
    m_hasCheckedAnimation = false;
}
