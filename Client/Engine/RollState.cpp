#include "pch.h"
#include "RollState.h"
#include "Player.h"
#include "PlayerMovement.h"
#include "GameObject.h"
#include "Transform.h"

void RollState::Enter(Player* player)
{
    player->PlayAnimation(PLAYER_STATE::ROLL);

    // ������ ������ ���� ���� ����
    auto movement = player->GetMovementComponent();
    m_rollDirection = movement->GetMoveDirection();
    if (m_rollDirection == Vec3::Zero) {
        m_rollDirection = -player->GetGameObject()->GetTransform()->GetLook();
    }

    m_rollTimer = 0.0f;
}

void RollState::Update(Player* player, float deltaTime)
{
    m_rollTimer += deltaTime;
    const float rollDuration = 0.5f;
    const float rollSpeed = 1000.0f;

    if (m_rollTimer >= rollDuration) {
        player->SetState(PLAYER_STATE::IDLE);
        return;
    }

    // ������ ���� ����� �������� ������ �̵�
    UpdateMovement(player, m_rollDirection * rollSpeed, deltaTime);
}

void RollState::Exit(Player* player)
{
	// ���� ���� �� �ʿ��� ����
}
