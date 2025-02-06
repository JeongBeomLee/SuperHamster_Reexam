#include "pch.h"
#include "FireState.h"
#include "Player.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Transform.h"
#include "ProjectileManager.h"
#include "PlayerMovement.h"

void FireState::Enter(Player* player)
{
	player->PlayAnimation(PLAYER_STATE::FIRE);
	m_hasCheckedAnimation = false;

    // �ѱ� ��ġ ��� (�����տ� ������ ���� ��ġ)
    auto playerObject = player->GetGameObject();
    if (!playerObject) return;

    // �� ������Ʈ ã��
    auto scene = GET_SINGLE(SceneManager)->GetActiveScene();
    auto gunObject = scene->GetGameObjectByName(L"defaultGun");
    if (!gunObject) return;

    // �ѱ� ��ġ�� ���� ���
    Vec3 muzzlePosition = gunObject->GetTransform()->GetWorldPosition();
    Vec3 fireDirection = player->GetGameObject()->GetTransform()->GetLook() * -1.f;
    fireDirection.Normalize();

    // ���� �ִ� �������� �Ѿ� ��ġ �̵�
	Vec3 offset = fireDirection * 50.f;
	muzzlePosition += offset;

    // �Ѿ� ���� �� �߻�
    GET_SINGLE(ProjectileManager)->SpawnProjectile(muzzlePosition, fireDirection);
}

void FireState::Update(Player* player, float deltaTime)
{
    auto animator = player->GetAnimator();
    if (!animator)
        return;

    // �ִϸ��̼��� �������� Ȯ��
    if (!m_hasCheckedAnimation && animator->IsAnimationFinished())
    {
        m_hasCheckedAnimation = true;
        player->SetState(PLAYER_STATE::AIM);
        return;
    }
}

void FireState::Exit(Player* player)
{
    m_hasCheckedAnimation = false;
}
