#include "pch.h"
#include "RollState.h"
#include "Player.h"
#include "CharacterMovement.h"
#include "GameObject.h"
#include "Transform.h"

void RollState::Enter(Player* player)
{
    player->PlayAnimation(PLAYER_STATE::ROLL);
    player->GetMovementComponent()->SetRollSpeed(1.5f);

    // ������ ������ ���� ���� ����
    auto movement = player->GetMovementComponent();
    m_rollDirection = movement->GetMoveDirection();
    if (m_rollDirection == Vec3::Zero) {
        m_rollDirection = -player->GetGameObject()->GetTransform()->GetLook();
    }

    player->SetInvincible(true);
    m_rollTimer = 0.0f;
}

void RollState::Update(Player* player, float deltaTime)
{
    m_rollTimer += deltaTime;
    const float rollDuration = 0.5f;

    if (m_rollTimer >= rollDuration) {
        player->SetState(PLAYER_STATE::IDLE);
        return;
    }

    // ������ ���� ����� �������� ������ �̵�
    UpdateMovement(player, m_rollDirection, deltaTime);
}

void RollState::Exit(Player* player)
{
	// ���� ���� �� �ʿ��� ����
	player->SetInvincible(false);
    player->GetMovementComponent()->SetRollSpeed(1.0f);
}
