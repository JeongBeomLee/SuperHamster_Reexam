#pragma once
#include "RemotePlayerState.h"
#include "Camera.h"
#include "SceneManager.h"
#include "Scene.h"
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
        // ī�޶� ���� ���� ���
        auto camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
        Vec3 forward = camera->GetTransform()->GetLook();
        Vec3 right = camera->GetTransform()->GetRight();

        // Y�� ���� ����
        forward.y = 0;
        right.y = 0;
        forward.Normalize();
        right.Normalize();

        moveDir = Vec3::Zero;
        if (inputData.inputFlags & InputFlags::UP)    
            moveDir += forward;
        if (inputData.inputFlags & InputFlags::DOWN)  
            moveDir -= forward;
        if (inputData.inputFlags & InputFlags::LEFT)  
            moveDir -= right;
        if (inputData.inputFlags & InputFlags::RIGHT) 
            moveDir += right;

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