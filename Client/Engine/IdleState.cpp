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

    // �÷��̾� ��ġ �α� ���
	if (INPUT->GetButtonDown(KEY_TYPE::KEY_1)) {
		auto transform = player->GetGameObject()->GetTransform();
		auto position = transform->GetWorldPosition();
		Logger::Instance().Debug("Player Position: ({}f, {}f, {}f)", position.x, position.y, position.z);
	}

    if (INPUT->GetButtonDown(KEY_TYPE::KEY_2)) {
        auto characterController = player->GetCharacterController();
        characterController->Teleport(Vec3(-1.1135427f, 310.f, -7.5789433f));
    }

	// Q�� ������ +z�������� 500��ŭ �̵�
    if (INPUT->GetButtonDown(KEY_TYPE::Q)) {
		auto transform = player->GetGameObject()->GetTransform();
		auto teleportPosition = transform->GetWorldPosition() + Vec3(0, 100, 500);

		auto characterController = player->GetCharacterController();
		characterController->Teleport(teleportPosition);
	}

    if (INPUT->GetButtonDown(KEY_TYPE::E)) {
        auto transform = player->GetGameObject()->GetTransform();
        auto teleportPosition = transform->GetWorldPosition() + Vec3(0, 100, -500);

        auto characterController = player->GetCharacterController();
        characterController->Teleport(teleportPosition);
    }

    if (INPUT->GetButtonDown(KEY_TYPE::Z)) {
        auto transform = player->GetGameObject()->GetTransform();
        auto teleportPosition = transform->GetWorldPosition() + Vec3(500, 100, 0);

        auto characterController = player->GetCharacterController();
        characterController->Teleport(teleportPosition);
    }

    if (INPUT->GetButtonDown(KEY_TYPE::C)) {
        auto transform = player->GetGameObject()->GetTransform();
        auto teleportPosition = transform->GetWorldPosition() + Vec3(-500, 100, 0);

        auto characterController = player->GetCharacterController();
        characterController->Teleport(teleportPosition);
    }
}

void IdleState::Exit(Player* player)
{
    // Idle ���� ���� �� �ʿ��� ����
}