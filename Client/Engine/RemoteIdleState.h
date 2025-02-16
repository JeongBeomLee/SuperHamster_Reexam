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
        // 원격 플레이어의 Idle 상태에서는 네트워크 입력만을 기다립니다
    }

    virtual void Exit(Player* player) override {}

    virtual void ProcessNetworkInput(Player* player, const NetworkInputData& inputData) override {
        // 이동 입력 확인
        if (inputData.inputFlags & (
            InputFlags::UP | InputFlags::DOWN |
            InputFlags::LEFT | InputFlags::RIGHT)) {
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

