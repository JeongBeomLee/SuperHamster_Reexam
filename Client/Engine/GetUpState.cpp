#include "pch.h"
#include "GetUpState.h"
#include "Player.h"
#include "Animator.h"

void GetUpState::Enter(Player* player)
{
    player->PlayAnimation(PLAYER_STATE::GETUP);
    m_hasCheckedAnimation = false;

    // 플레이어가 보는 방향의 반대로 밀려나기
    auto controller = player->GetCharacterController();
    if (controller) {
        // 플레이어의 현재 전방 벡터를 가져옵니다
        Vec3 forward = player->GetGameObject()->GetTransform()->GetLook();
        forward.Normalize();

        // 전방 벡터의 반대 방향으로 100 유닛만큼 이동할 위치를 계산합니다
        Vec3 currentPos = player->GetGameObject()->GetTransform()->GetWorldPosition();
        Vec3 knockbackPos = currentPos + (forward * 100.f);  // forward가 negative look이므로 더하기 사용
        knockbackPos.y += 100.0f;

        // 캐릭터 컨트롤러를 통해 새로운 위치로 순간 이동합니다
        controller->Teleport(knockbackPos);

        Logger::Instance().Debug("플레이어가 넉백됨: ({}, {}, {})",
            knockbackPos.x, knockbackPos.y, knockbackPos.z);
    }

    Logger::Instance().Debug("플레이어가 일어나기 상태로 전환");
}

void GetUpState::Update(Player* player, float deltaTime)
{
    auto animator = player->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        player->SetState(PLAYER_STATE::IDLE);
        Logger::Instance().Debug("일어나기 애니메이션 종료, 대기 상태로 전환");
    }
}

void GetUpState::Exit(Player* player)
{
    m_hasCheckedAnimation = false;
	player->SetInvincible(false);
}
