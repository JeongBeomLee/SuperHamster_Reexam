#include "pch.h"
#include "IdleState.h"
#include "Player.h"
#include "Input.h"

void IdleState::Enter(Player* player)
{
    player->PlayAnimation(PLAYER_STATE::IDLE);
}

void IdleState::Update(Player* player, float deltaTime)
{
    if (IsMovementInput()) {
        player->SetState(PLAYER_STATE::RUN);
        return;
    }

    if (INPUT->GetButton(KEY_TYPE::A)) {
        player->SetState(PLAYER_STATE::AIM);
        return;
    }

    if (INPUT->GetButtonDown(KEY_TYPE::SPACE)) {
        player->SetState(PLAYER_STATE::ROLL);
        return;
    }
}

void IdleState::Exit(Player* player)
{
    // Idle 상태 종료 시 필요한 로직
}