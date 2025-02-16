#pragma once
#include "RemotePlayerState.h"
class RemoteAimState : public RemotePlayerState {
public:
    virtual void Enter(Player* player) override {
        player->PlayAnimation(PLAYER_STATE::AIM);
        player->GetMovementComponent()->StopMovement();
    }

    virtual void Update(Player* player, float deltaTime) override {
        // 조준 상태 유지
    }

    virtual void Exit(Player* player) override {}

    virtual void ProcessNetworkInput(Player* player, const NetworkInputData& inputData) override {
        // 조준 해제
        if (!(inputData.inputFlags & InputFlags::A)) {
            player->SetState(REMOTE_PLAYER_STATE::IDLE);
            return;
        }

        // 발사
        if (inputData.inputFlags & InputFlags::S) {
            player->SetState(REMOTE_PLAYER_STATE::FIRE);
            return;
        }

        // 조준 중 회전
        Vec3 aimDir = Vec3::Zero;
        if (inputData.inputFlags & InputFlags::UP)    aimDir += Vec3(0.f, 0.f, 1.f);
        if (inputData.inputFlags & InputFlags::DOWN)  aimDir += Vec3(0.f, 0.f, -1.f);
        if (inputData.inputFlags & InputFlags::LEFT)  aimDir += Vec3(-1.f, 0.f, 0.f);
        if (inputData.inputFlags & InputFlags::RIGHT) aimDir += Vec3(1.f, 0.f, 0.f);

        if (aimDir != Vec3::Zero) {
            aimDir.Normalize();
            player->GetMovementComponent()->SmoothRotation(aimDir, DELTA_TIME);
        }
    }
};