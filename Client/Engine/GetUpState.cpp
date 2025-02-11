#include "pch.h"
#include "GetUpState.h"
#include "Player.h"
#include "Animator.h"

void GetUpState::Enter(Player* player)
{
    player->PlayAnimation(PLAYER_STATE::GETUP);
    m_hasCheckedAnimation = false;

    // �÷��̾ ���� ������ �ݴ�� �з�����
    auto controller = player->GetCharacterController();
    if (controller) {
        // �÷��̾��� ���� ���� ���͸� �����ɴϴ�
        Vec3 forward = player->GetGameObject()->GetTransform()->GetLook();
        forward.Normalize();

        // ���� ������ �ݴ� �������� 100 ���ָ�ŭ �̵��� ��ġ�� ����մϴ�
        Vec3 currentPos = player->GetGameObject()->GetTransform()->GetWorldPosition();
        Vec3 knockbackPos = currentPos + (forward * 100.f);  // forward�� negative look�̹Ƿ� ���ϱ� ���
        knockbackPos.y += 100.0f;

        // ĳ���� ��Ʈ�ѷ��� ���� ���ο� ��ġ�� ���� �̵��մϴ�
        controller->Teleport(knockbackPos);

        Logger::Instance().Debug("�÷��̾ �˹��: ({}, {}, {})",
            knockbackPos.x, knockbackPos.y, knockbackPos.z);
    }

    Logger::Instance().Debug("�÷��̾ �Ͼ�� ���·� ��ȯ");
}

void GetUpState::Update(Player* player, float deltaTime)
{
    auto animator = player->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        player->SetState(PLAYER_STATE::IDLE);
        Logger::Instance().Debug("�Ͼ�� �ִϸ��̼� ����, ��� ���·� ��ȯ");
    }
}

void GetUpState::Exit(Player* player)
{
    m_hasCheckedAnimation = false;
	player->SetInvincible(false);
}
