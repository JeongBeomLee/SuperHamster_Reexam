#pragma once
#include "RemotePlayerState.h"
class RemoteGetUpState : public RemotePlayerState {
public:
    virtual void Enter(Player* player) override {
        player->PlayAnimation(PLAYER_STATE::GETUP);
        m_hasCheckedAnimation = false;

        // 넉백 효과 적용
        auto controller = player->GetCharacterController();
        if (controller) {
            Vec3 forward = player->GetGameObject()->GetTransform()->GetLook();
            Vec3 currentPos = player->GetGameObject()->GetTransform()->GetWorldPosition();
            Vec3 knockbackPos = currentPos + (forward * 100.f);
            knockbackPos.y += 100.0f;
            controller->Teleport(knockbackPos);
        }
    }

    virtual void Update(Player* player, float deltaTime) override {
        auto animator = player->GetAnimator();
        if (!animator) return;

        if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
            m_hasCheckedAnimation = true;
            player->SetState(REMOTE_PLAYER_STATE::IDLE);
        }
    }

    virtual void Exit(Player* player) override {
        m_hasCheckedAnimation = false;
        player->SetInvincible(false);
    }

    virtual void ProcessNetworkInput(Player* player, const NetworkInputData& inputData) override {
        // 일어나는 중에는 추가 입력을 무시
    }

private:
    bool m_hasCheckedAnimation = false;
};