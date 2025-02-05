#include "pch.h"
#include "IdleState.h"
#include "Player.h"
#include "Input.h"

void IdleState::Enter(Player* player)
{
    player->PlayAnimation(PLAYER_STATE::IDLE);
    player->GetMovementComponent()->StopMovement();
}

void IdleState::Update(Player* player, float deltaTime)
{
    if (IsMovementInput()) {
        player->SetState(PLAYER_STATE::RUN);
        return;
    }

    if (INPUT->GetButton(KEY_TYPE::A)) {
        player->SetState(PLAYER_STATE::AIM);
        player->GetMovementComponent()->StopMovement();
        return;
    }

    if (INPUT->GetButtonDown(KEY_TYPE::SPACE)) {
        player->SetState(PLAYER_STATE::ROLL);
        return;
    }
}

void IdleState::Exit(Player* player)
{
    // Idle ���� ���� �� �ʿ��� ����
}