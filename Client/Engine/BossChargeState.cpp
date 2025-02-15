#include "pch.h"
#include "BossChargeState.h"
#include "Boss.h"
#include "Resources.h"
#include "SoundSystem.h"

void BossChargeState::Enter(Boss* boss)
{
    boss->PlayAnimation(BOSS_STATE::CHARGE);
    m_hasStartedCharge = false;
    m_prepareTimer = 0.0f;

    // Ÿ�� ��ġ�� ���� ���� ���
    if (Player* target = boss->GetTarget()) {
        m_targetPosition = target->GetGameObject()->GetTransform()->GetWorldPosition();

        Vec3 bossPos = boss->GetGameObject()->GetTransform()->GetWorldPosition();
        m_chargeDirection = m_targetPosition - bossPos;
        m_chargeDirection.y = 0.0f;
        m_chargeDirection.Normalize();

        // Ÿ���� ���� ��� ȸ��
        boss->GetGameObject()->GetTransform()->LookAt(m_chargeDirection * -1.f);

        auto movement = boss->GetGameObject()->GetCharacterMovement();
        if (movement) {
            movement->SetMoveSpeed(boss->GetChargeSpeed());
        }
    }

    auto sound = GET_SINGLE(Resources)->Get<Sound>(L"BossCharge");
    if (sound) {
        GET_SINGLE(SoundSystem)->Play3D(sound, boss->GetGameObject()->GetTransform()->GetLocalPosition());
    }
    Logger::Instance().Debug("������ Charge ���·� ����");
}

void BossChargeState::Update(Boss* boss, float deltaTime)
{
    if (!m_hasStartedCharge) {
        m_prepareTimer += deltaTime;
        if (m_prepareTimer >= PREPARE_TIME) {
            m_hasStartedCharge = true;
        }
        return;
    }

    auto controller = boss->GetGameObject()->GetCharacterController();
    if (!controller) return;

    // ���� �̵�
    controller->Move(m_chargeDirection * boss->GetChargeSpeed() * deltaTime, deltaTime);

    // ��ǥ ���� ���� üũ
    Vec3 currentPos = boss->GetGameObject()->GetTransform()->GetWorldPosition();
    Vec3 toTarget = m_targetPosition - currentPos;
    toTarget.y = 0.0f;

    // ��ǥ ������ �����ưų� ����� ��������ٸ�
    if (toTarget.Dot(m_chargeDirection) < 0.0f || toTarget.Length() < 150.0f) {
        boss->SetState(BOSS_STATE::DOWN_SMASH_ATTACK);
    }
}

void BossChargeState::Exit(Boss* boss)
{
}
