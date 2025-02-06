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

    // 총구 위치 계산 (오른손에 부착된 총의 위치)
    auto playerObject = player->GetGameObject();
    if (!playerObject) return;

    // 총 오브젝트 찾기
    auto scene = GET_SINGLE(SceneManager)->GetActiveScene();
    auto gunObject = scene->GetGameObjectByName(L"defaultGun");
    if (!gunObject) return;

    // 총구 위치와 방향 계산
    Vec3 muzzlePosition = gunObject->GetTransform()->GetWorldPosition();
    Vec3 fireDirection = player->GetGameObject()->GetTransform()->GetLook() * -1.f;
    fireDirection.Normalize();

    // 보고 있는 방향으로 총알 위치 이동
	Vec3 offset = fireDirection * 50.f;
	muzzlePosition += offset;

    // 총알 생성 및 발사
    GET_SINGLE(ProjectileManager)->SpawnProjectile(muzzlePosition, fireDirection);
}

void FireState::Update(Player* player, float deltaTime)
{
    auto animator = player->GetAnimator();
    if (!animator)
        return;

    // 애니메이션이 끝났는지 확인
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
