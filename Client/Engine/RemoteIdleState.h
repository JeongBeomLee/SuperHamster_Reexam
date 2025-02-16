#pragma once
#include "RemotePlayerState.h"
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

