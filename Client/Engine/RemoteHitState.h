#pragma once
#include "RemotePlayerState.h"
class RemoteHitState : public RemotePlayerState {
public:
    virtual void Enter(Player* player) override {
        player->PlayAnimation(PLAYER_STATE::HIT);
        m_hasCheckedAnimation = false;
        player->GetMovementComponent()->StopMovement();
        player->SetInvincible(true);
    }

    virtual void Update(Player* player, float deltaTime) override {
        auto animator = player->GetAnimator();
        if (!animator) return;

        if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
            m_hasCheckedAnimation = true;
            player->SetState(REMOTE_PLAYER_STATE::GETUP);
        }
    }

    virtual void Exit(Player* player) override {
        m_hasCheckedAnimation = false;
    }

    virtual void ProcessNetworkInput(Player* player, const NetworkInputData& inputData) override {
        // 피격 중에는 추가 입력을 무시
    }

private:
    bool m_hasCheckedAnimation = false;
};


