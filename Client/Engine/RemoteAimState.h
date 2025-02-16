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
        // ���� ���� ����
    }

    virtual void Exit(Player* player) override {}

    virtual void ProcessNetworkInput(Player* player, const NetworkInputData& inputData) override {
        // ���� ����
        if (!(inputData.inputFlags & InputFlags::A)) {
            player->SetState(REMOTE_PLAYER_STATE::IDLE);
            return;
        }

        // �߻�
        if (inputData.inputFlags & InputFlags::S) {
            player->SetState(REMOTE_PLAYER_STATE::FIRE);
            return;
        }

        // ī�޶� ���� ���� ���
        auto camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
        Vec3 forward = camera->GetTransform()->GetLook();
        Vec3 right = camera->GetTransform()->GetRight();

        // Y�� ���� ����
        forward.y = 0;
        right.y = 0;
        forward.Normalize();
        right.Normalize();

        // ���� �� ȸ��
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