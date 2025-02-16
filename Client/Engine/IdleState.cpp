#include "pch.h"
#include "IdleState.h"
#include "Player.h"
#include "Input.h"
#include "Transform.h"

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

    // 플레이어 위치 로그 출력
	if (INPUT->GetButtonDown(KEY_TYPE::KEY_1)) {
		auto transform = player->GetGameObject()->GetTransform();
		auto position = transform->GetWorldPosition();
		Logger::Instance().Debug("Player Position: ({}f, {}f, {}f)", position.x, position.y, position.z);
	}

    if (INPUT->GetButtonDown(KEY_TYPE::KEY_2)) {
        auto characterController = player->GetCharacterController();
        characterController->Teleport(Vec3(-1.1135427f, 310.f, -7.5789433f));
    }
}

void IdleState::Exit(Player* player)
{
    // Idle 상태 종료 시 필요한 로직
}