#include "pch.h"
#include "CharaterController.h"
#include "Transform.h"
#include "Timer.h"
#include "PhysicsTypes.h"
#include "Engine.h"

CharaterController::CharaterController() : Component(COMPONENT_TYPE::CHARACTER_CONTROLLER)
{
}

CharaterController::~CharaterController()
{
    if (m_controller) {
        m_controller->release();
        m_controller = nullptr;
    }
}

void CharaterController::Initialize()
{
    // ĸ�� ��Ʈ�ѷ� ����
    auto transform = GetTransform();
    auto position = transform->GetLocalPosition();

    auto physicsObject = PHYSICS_ENGINE->CreateCapsuleController(
        PxVec3(position.x, position.y, position.z),
        0.5f,   // ������
        2.0f,   // ����
        CollisionGroup::Character,
        CollisionGroup::Default | CollisionGroup::Ground |
        CollisionGroup::Obstacle | CollisionGroup::Character
    );

    if (!physicsObject) {
        Logger::Instance().Error("ĳ���� ��Ʈ�ѷ� ���� ����");
        return;
    }

	// TODO: ĳ���� ��Ʈ�ѷ� ID ����
    m_controller = PHYSICS_ENGINE->GetControllerManager()->getController(0);
    Logger::Instance().Debug("ĳ���� ��Ʈ�ѷ� �ʱ�ȭ��");
}

void CharaterController::Update()
{
    if (!m_controller) return;

    //HandleInput(deltaTime);
    UpdateTransform();
}

void CharaterController::Move(const XMFLOAT3& displacement, float minDist, float deltaTime)
{
    auto obstacleContext = PHYSICS_ENGINE->GetObstacleContext();
    if (!m_controller) return;
    if (!obstacleContext) return;

    PxVec3 disp(displacement.x, displacement.y, displacement.z);

    // ĳ���� �̵� ���� ����
    PxControllerFilters filters;
    PxFilterData filterData(
        static_cast<PxU32>(CollisionGroup::Character),
        static_cast<PxU32>(CollisionGroup::Default | CollisionGroup::Ground | CollisionGroup::Obstacle),
        0, 0);
    filters.mFilterData = &filterData;

    // �̵� ����
    PxControllerCollisionFlags collisionFlags =
        m_controller->move(disp, minDist, deltaTime, filters, obstacleContext);

    // ���� ���� ���� ���� - ��Ʈ �������� ����
    m_isOnGround = (collisionFlags.isSet(PxControllerCollisionFlag::eCOLLISION_DOWN));
}

const PxExtendedVec3& CharaterController::GetPosition() const
{
    static PxExtendedVec3 defaultPos(0.0f);
    return m_controller ? m_controller->getPosition() : defaultPos;
}

void CharaterController::SetStepOffset(float offset)
{
    if (m_controller) {
        m_controller->setStepOffset(offset);
    }
}

void CharaterController::SetSlopeLimit(float slopeLimit)
{
    if (m_controller) {
        m_controller->setSlopeLimit(cosf(slopeLimit));
    }
}

void CharaterController::SetContactOffset(float offset)
{
    if (m_controller) {
        m_controller->setContactOffset(offset);
    }
}

void CharaterController::UpdateTransform()
{
    if (!m_controller) return;

    // ��Ʈ�ѷ��� ��ġ�� Transform ������Ʈ�� �ݿ�
    const PxExtendedVec3& position = m_controller->getPosition();

    auto transform = GetTransform();
    transform->SetLocalPosition(XMFLOAT3(
        static_cast<float>(position.x),
        static_cast<float>(position.y),
        static_cast<float>(position.z)
    ));
}
