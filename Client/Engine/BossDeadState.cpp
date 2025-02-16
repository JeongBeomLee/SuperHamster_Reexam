#include "pch.h"
#include "BossDeadState.h"
#include "Boss.h"
#include "CharacterMovement.h"
#include "Resources.h"
#include "SoundSystem.h"
#include "EventTypes.h"
#include "EventManager.h"

void BossDeadState::Enter(Boss* boss)
{
    boss->PlayAnimation(BOSS_STATE::DEAD);
    m_hasCheckedAnimation = false;

    // ���� �� �̵� ����
    auto movement = boss->GetGameObject()->GetCharacterMovement();
    if (movement) {
        movement->StopMovement();
    }

    auto sound = GET_SINGLE(Resources)->Get<Sound>(L"BossDead");
    if (sound) {
        GET_SINGLE(SoundSystem)->Play3D(sound, boss->GetGameObject()->GetTransform()->GetLocalPosition());
    }
}

void BossDeadState::Update(Boss* boss, float deltaTime)
{
    auto animator = boss->GetGameObject()->GetAnimator();
    if (!animator) return;

    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        // ��� �ִϸ��̼��� ������ ���� ������Ʈ ��Ȱ��ȭ
        boss->GetGameObject()->SetActive(false);

        // ���� ����(���ɸ��� ��¥ �����ֱ�)
        if (auto controller = boss->GetGameObject()->GetCharacterController()) {
            controller->GetController()->setPosition(PxExtendedVec3(0.f, -9999.f, 0.f));
        }

		// ���� ����
        GET_SINGLE(SoundSystem)->StopAll();

        // �� ü���� �̺�Ʈ
		Event::SceneChangeEvent event;
        event.newScene = SceneManager::SceneType::GAME_CLEAR;
		GET_SINGLE(EventManager)->Publish(event);
    }
}

void BossDeadState::Exit(Boss* boss)
{
    m_hasCheckedAnimation = false;
}
