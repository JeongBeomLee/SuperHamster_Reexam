#pragma once
#include "RemotePlayerState.h"
class RemoteRunState : public RemotePlayerState {
public:
    virtual void Enter(Player* player) override {
        player->PlayAnimation(PLAYER_STATE::RUN);
    }

    virtual void Update(Player* player, float deltaTime) override {
        // ���� �̵� �������� ��� �̵�
        if (moveDir != Vec3::Zero) {
            player->GetMovementComponent()->SetMoveDirection(moveDir);
        }
    }

    virtual void Exit(Player* player) override {
        player->GetMovementComponent()->StopMovement();
    }

    virtual void ProcessNetworkInput(Player* player, const NetworkInputData& inputData) override {
        // �̵� ���� ���
        moveDir = Vec3::Zero;
        if (inputData.inputFlags & InputFlags::UP)    moveDir += Vec3(0.f, 0.f, 1.f);
        if (inputData.inputFlags & InputFlags::DOWN)  moveDir += Vec3(0.f, 0.f, -1.f);
        if (inputData.inputFlags & InputFlags::LEFT)  moveDir += Vec3(-1.f, 0.f, 0.f);
        if (inputData.inputFlags & InputFlags::RIGHT) moveDir += Vec3(1.f, 0.f, 0.f);

        if (moveDir != Vec3::Zero) {
            moveDir.Normalize();
        }
        else {
            // �̵� �Է��� ������ Idle ���·�
            player->SetState(REMOTE_PLAYER_STATE::IDLE);
            return;
        }

        // �ٸ� ���·��� ��ȯ üũ
        if (inputData.inputFlags & InputFlags::A) {
            player->SetState(REMOTE_PLAYER_STATE::AIM);
            return;
        }

        if (inputData.inputFlags & InputFlags::SPACE) {
            player->SetState(REMOTE_PLAYER_STATE::ROLL);
            return;
        }
    }

private:
    Vec3 moveDir = Vec3::Zero;
};