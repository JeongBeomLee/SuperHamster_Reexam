#pragma once
#include "RemotePlayerState.h"
class RemoteRunState : public RemotePlayerState {
public:
    virtual void Enter(Player* player) override {
        player->PlayAnimation(PLAYER_STATE::RUN);
    }

    virtual void Update(Player* player, float deltaTime) override {
        // 현재 이동 방향으로 계속 이동
        if (moveDir != Vec3::Zero) {
            player->GetMovementComponent()->SetMoveDirection(moveDir);
        }
    }

    virtual void Exit(Player* player) override {
        player->GetMovementComponent()->StopMovement();
    }

    virtual void ProcessNetworkInput(Player* player, const NetworkInputData& inputData) override {
        // 이동 방향 계산
        moveDir = Vec3::Zero;
        if (inputData.inputFlags & InputFlags::UP)    moveDir += Vec3(0.f, 0.f, 1.f);
        if (inputData.inputFlags & InputFlags::DOWN)  moveDir += Vec3(0.f, 0.f, -1.f);
        if (inputData.inputFlags & InputFlags::LEFT)  moveDir += Vec3(-1.f, 0.f, 0.f);
        if (inputData.inputFlags & InputFlags::RIGHT) moveDir += Vec3(1.f, 0.f, 0.f);

        if (moveDir != Vec3::Zero) {
            moveDir.Normalize();
        }
        else {
            // 이동 입력이 없으면 Idle 상태로
            player->SetState(REMOTE_PLAYER_STATE::IDLE);
            return;
        }

        // 다른 상태로의 전환 체크
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