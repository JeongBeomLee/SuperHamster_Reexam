#pragma once
#include "RemotePlayerState.h"
class RemoteRollState :
    public RemotePlayerState
{
public:
    virtual void Enter(Player* player) override {
        player->PlayAnimation(PLAYER_STATE::ROLL);
        player->GetMovementComponent()->SetRollSpeed(1.5f);
        player->SetInvincible(true);
        m_rollTimer = 0.0f;

        // ������ ���� ����
        m_rollDirection = player->GetMovementComponent()->GetCurrentDirection();
        if (m_rollDirection == Vec3::Zero) {
            m_rollDirection = -player->GetGameObject()->GetTransform()->GetLook();
        }
    }

    virtual void Update(Player* player, float deltaTime) override {
        m_rollTimer += deltaTime;
        const float rollDuration = 0.5f;

        if (m_rollTimer >= rollDuration) {
            player->SetState(REMOTE_PLAYER_STATE::IDLE);
            return;
        }

        // ����� �������� ��� ������
        player->GetMovementComponent()->SetMoveDirection(m_rollDirection);
    }

    virtual void Exit(Player* player) override {
        player->SetInvincible(false);
        player->GetMovementComponent()->SetRollSpeed(1.0f);
    }

    virtual void ProcessNetworkInput(Player* player, const NetworkInputData& inputData) override {
        // ������ �߿��� �߰� �Է��� ����
    }

private:
    Vec3 m_rollDirection = Vec3::Zero;
    float m_rollTimer = 0.0f;
};

