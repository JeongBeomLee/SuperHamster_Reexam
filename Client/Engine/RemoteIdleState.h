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
        // 원격 플레이어의 Idle 상태에서는 네트워크 입력만을 기다립니다
    }

    virtual void Exit(Player* player) override {}

    virtual void ProcessNetworkInput(Player* player, const NetworkInputData& inputData) override {
        // 이동 입력 확인
        if (inputData.inputFlags & (
            InputFlags::UP | InputFlags::DOWN |
            InputFlags::LEFT | InputFlags::RIGHT)) {

            // 카메라 기준 방향 계산
            auto camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
            Vec3 forward = camera->GetTransform()->GetLook();
            Vec3 right = camera->GetTransform()->GetRight();

            // Y축 영향 제거
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

        // 조준 입력 확인
        if (inputData.inputFlags & InputFlags::A) {
            player->SetState(REMOTE_PLAYER_STATE::AIM);
            return;
        }

        // 구르기 입력 확인
        if (inputData.inputFlags & InputFlags::SPACE) {
            player->SetState(REMOTE_PLAYER_STATE::ROLL);
            return;
        }
    }
};

