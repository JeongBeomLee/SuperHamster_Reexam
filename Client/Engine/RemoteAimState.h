#pragma once
#include "RemotePlayerState.h"
#include "Camera.h"
#include "SceneManager.h"
#include "Scene.h"
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

        // 카메라 기준 방향 계산
        auto camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
        Vec3 forward = camera->GetTransform()->GetLook();
        Vec3 right = camera->GetTransform()->GetRight();

        // Y축 영향 제거
        forward.y = 0;
        right.y = 0;
        forward.Normalize();
        right.Normalize();

        // 조준 중 회전
        Vec3 aimDir = Vec3::Zero;
        if (inputData.inputFlags & InputFlags::UP)    
            aimDir += forward;
        if (inputData.inputFlags & InputFlags::DOWN)  
            aimDir -= forward;
        if (inputData.inputFlags & InputFlags::LEFT)  
            aimDir -= right;
        if (inputData.inputFlags & InputFlags::RIGHT) 
            aimDir += right;

        if (aimDir != Vec3::Zero) {
            aimDir.Normalize();
            player->GetMovementComponent()->SmoothRotation(aimDir, DELTA_TIME);
        }
    }
};