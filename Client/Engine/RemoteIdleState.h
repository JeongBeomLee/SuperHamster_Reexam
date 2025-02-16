#pragma once
#include "RemotePlayerState.h"
#include "RemoteRunState.h"
#include "Camera.h"
class RemoteIdleState : public RemotePlayerState
{
public:
    virtual void Enter(Player* player) override {
        player->PlayAnimation(PLAYER_STATE::IDLE);
        player->GetMovementComponent()->StopMovement();
    }

    virtual void Update(Player* player, float deltaTime) override {
        // ���� �÷��̾��� Idle ���¿����� ��Ʈ��ũ �Է¸��� ��ٸ��ϴ�
    }

    virtual void Exit(Player* player) override {}

    virtual void ProcessNetworkInput(Player* player, const NetworkInputData& inputData) override {
        // �̵� �Է� Ȯ��
        if (inputData.inputFlags & (
            InputFlags::UP | InputFlags::DOWN |
            InputFlags::LEFT | InputFlags::RIGHT)) {

            // ī�޶� ���� ���� ���
            auto camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
            Vec3 forward = camera->GetTransform()->GetLook();
            Vec3 right = camera->GetTransform()->GetRight();

            // Y�� ���� ����
            forward.y = 0;
            right.y = 0;
            forward.Normalize();
            right.Normalize();

            RemoteRunState::moveDir = Vec3::Zero;
            if (inputData.inputFlags & InputFlags::UP)
                RemoteRunState::moveDir += forward;
            if (inputData.inputFlags & InputFlags::DOWN)
                RemoteRunState::moveDir -= forward;
            if (inputData.inputFlags & InputFlags::LEFT)
                RemoteRunState::moveDir -= right;
            if (inputData.inputFlags & InputFlags::RIGHT)
                RemoteRunState::moveDir += right;

            if (RemoteRunState::moveDir != Vec3::Zero) {
                RemoteRunState::moveDir.Normalize();
                player->GetMovementComponent()->SetMoveDirection(RemoteRunState::moveDir);
            }
            player->SetState(REMOTE_PLAYER_STATE::RUN);
            return;
        }

        // ���� �Է� Ȯ��
        if (inputData.inputFlags & InputFlags::A) {
            player->SetState(REMOTE_PLAYER_STATE::AIM);
            return;
        }

        // ������ �Է� Ȯ��
        if (inputData.inputFlags & InputFlags::SPACE) {
            player->SetState(REMOTE_PLAYER_STATE::ROLL);
            return;
        }
    }
};

