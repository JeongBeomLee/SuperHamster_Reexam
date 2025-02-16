#pragma once
#include "RemotePlayerState.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Transform.h"
#include "RemoteProjectileManager.h"
#include "CharacterMovement.h"
#include "Resources.h"
#include "SoundSystem.h"

class RemoteFireState : public RemotePlayerState {
public:
    virtual void Enter(Player* player) override {
        player->PlayAnimation(PLAYER_STATE::FIRE);
        m_hasCheckedAnimation = false;

        // 총 오브젝트 찾기
        auto scene = GET_SINGLE(SceneManager)->GetActiveScene();
        auto gunObject = scene->GetGameObjectByName(L"RemotePlayerGun");
        if (!gunObject) return;

        // 총구 위치와 방향 계산
        Vec3 muzzlePosition = gunObject->GetTransform()->GetWorldPosition();
        Vec3 fireDirection = player->GetGameObject()->GetTransform()->GetLook() * -1.f;
        fireDirection.Normalize();

        // 보고 있는 방향으로 총알 위치 이동
        Vec3 offset = fireDirection * 50.f;
        muzzlePosition += offset;

        // 총알 생성 및 발사
        GET_SINGLE(RemoteProjectileManager)->SpawnProjectile(muzzlePosition, fireDirection);

        // 총알 발사 사운드 재생
        auto sound = GET_SINGLE(Resources)->Get<Sound>(L"LaserShot");
        if (sound) {
            GET_SINGLE(SoundSystem)->Play3D(sound, muzzlePosition);
        }
    }

    virtual void Update(Player* player, float deltaTime) override {
        auto animator = player->GetAnimator();
        if (!animator) return;

        if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
            m_hasCheckedAnimation = true;
            player->SetState(REMOTE_PLAYER_STATE::AIM);
        }
    }

    virtual void Exit(Player* player) override {
        m_hasCheckedAnimation = false;
    }

    virtual void ProcessNetworkInput(Player* player, const NetworkInputData& inputData) override {
        // 발사 중에는 추가 입력을 무시
    }

private:
    bool m_hasCheckedAnimation = false;
};