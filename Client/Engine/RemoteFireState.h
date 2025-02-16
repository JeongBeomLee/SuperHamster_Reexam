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

        // �� ������Ʈ ã��
        auto scene = GET_SINGLE(SceneManager)->GetActiveScene();
        auto gunObject = scene->GetGameObjectByName(L"RemotePlayerGun");
        if (!gunObject) return;

        // �ѱ� ��ġ�� ���� ���
        Vec3 muzzlePosition = gunObject->GetTransform()->GetWorldPosition();
        Vec3 fireDirection = player->GetGameObject()->GetTransform()->GetLook() * -1.f;
        fireDirection.Normalize();

        // ���� �ִ� �������� �Ѿ� ��ġ �̵�
        Vec3 offset = fireDirection * 50.f;
        muzzlePosition += offset;

        // �Ѿ� ���� �� �߻�
        GET_SINGLE(RemoteProjectileManager)->SpawnProjectile(muzzlePosition, fireDirection);

        // �Ѿ� �߻� ���� ���
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
        // �߻� �߿��� �߰� �Է��� ����
    }

private:
    bool m_hasCheckedAnimation = false;
};