#include "pch.h"
#include "RunState.h"
#include "Player.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "Transform.h"
#include "Input.h"
#include "Resources.h"
#include "SoundSystem.h"

void RunState::Enter(Player* player)
{
    player->PlayAnimation(PLAYER_STATE::RUN);

	/*Vec3 position = { 
		static_cast<float>(player->GetCharacterController()->GetPosition().x),
        static_cast<float>(player->GetCharacterController()->GetPosition().y), 
        static_cast<float>(player->GetCharacterController()->GetPosition().z) };

    auto sound = GET_SINGLE(Resources)->Get<Sound>(L"Footsteps");
    if (sound) {
        sound->SetVolume(80.f);
		sound->SetLoop(true);
        GET_SINGLE(SoundSystem)->Play3D(sound, position);
    }*/
}

void RunState::Update(Player* player, float deltaTime)
{
    if (!IsMovementInput()) {
        player->SetState(PLAYER_STATE::IDLE);
        return;
    }

    Vec3 moveDir = GetTargetDirection();
    if (moveDir != Vec3::Zero) {
        moveDir.Normalize();
        UpdateMovement(player, moveDir, deltaTime);
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

void RunState::Exit(Player* player)
{
    // Run 상태 종료 시 필요한 로직
    /*auto sound = GET_SINGLE(Resources)->Get<Sound>(L"Footsteps");
    if (sound) {
        sound->SetVolume(10.f);
        GET_SINGLE(SoundSystem)->Stop(sound);
    }*/
}