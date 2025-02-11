#include "pch.h"
#include "CharacterController.h"
#include "Transform.h"
#include "Timer.h"
#include "PhysicsTypes.h"
#include "Engine.h"
#include "SoundSystem.h"

CharacterController::CharacterController() : Component(COMPONENT_TYPE::CHARACTER_CONTROLLER)
{
}

CharacterController::~CharacterController()
{
    if (m_controller) {
        m_controller->release();
        m_controller = nullptr;
    }
}

void CharacterController::Initialize()
{
    // ĸ�� ��Ʈ�ѷ� ����
    auto transform = GetTransform();
    auto position = transform->GetLocalPosition();

    auto physicsObject = PHYSICS_ENGINE->CreateCapsuleController(
        PxVec3(position.x, position.y, position.z),
        40.0f,   // ������
        150.0f,   // ����
        CollisionGroup::Player,
        CollisionGroup::Default | CollisionGroup::Ground |
		CollisionGroup::Obstacle | CollisionGroup::Player | 
		CollisionGroup::Enemy | CollisionGroup::Trigger
    );

    if (!physicsObject) {
        Logger::Instance().Error("ĳ���� ��Ʈ�ѷ� ���� ����");
        return;
    }

	// TODO: ĳ���� ��Ʈ�ѷ� ID ����
    m_controller = PHYSICS_ENGINE->GetControllerManager()->getController(0);
    m_controller->setUserData(GetGameObject().get());
	m_controller->getActor()->userData = GetGameObject().get();

    Logger::Instance().Debug("ĳ���� ��Ʈ�ѷ� �ʱ�ȭ��");
}

void CharacterController::Update()
{
    if (!m_controller) return;

    float deltaTime = DELTA_TIME;

    // �߷� ����
    ApplyGravity(deltaTime);

    // Transform ������Ʈ ������Ʈ
    UpdateTransform();
}

void CharacterController::Move(const Vec3& displacement, float deltaTime)
{
    auto obstacleContext = PHYSICS_ENGINE->GetObstacleContext();
    if (!m_controller || !obstacleContext) return;

    PxVec3 disp(displacement.x, displacement.y, displacement.z);

    // �̵� �Ÿ� ���
    const float minDist = 0.001f;

    // ĳ���� �̵� ���� ����
    PxControllerFilters filters;
    PxFilterData filterData;
    filterData.word0 = static_cast<PxU32>(CollisionGroup::Player);
    filterData.word1 = static_cast<PxU32>(CollisionGroup::Default |
        CollisionGroup::Ground |
        CollisionGroup::Obstacle);
    filters.mFilterData = &filterData;

    // �̵� ����
    PxControllerCollisionFlags collisionFlags =
        m_controller->move(disp, minDist, deltaTime, filters, obstacleContext);

    // ���� ���� ���� ������Ʈ
    m_isGrounded = (collisionFlags.isSet(PxControllerCollisionFlag::eCOLLISION_DOWN));
}

void CharacterController::Teleport(const Vec3& position)
{
	if (!m_controller) return;
	m_controller->setPosition(PxExtendedVec3(
		position.x,
		position.y,
		position.z
	));
}

void CharacterController::ApplyGravity(float deltaTime)
{
    if (!m_controller || m_isGrounded) return;

    // �߷� ����
    Vec3 gravityMove(0.0f, m_gravity * deltaTime, 0.0f);
    Move(gravityMove, deltaTime);
}

const PxExtendedVec3& CharacterController::GetPosition() const
{
    static PxExtendedVec3 defaultPos(0.0f);
    return m_controller ? m_controller->getPosition() : defaultPos;
}

void CharacterController::SetStepOffset(float offset)
{
    if (m_controller) {
        m_controller->setStepOffset(offset);
    }
}

void CharacterController::SetSlopeLimit(float slopeLimit)
{
    if (m_controller) {
        m_controller->setSlopeLimit(cosf(slopeLimit));
    }
}

void CharacterController::SetContactOffset(float offset)
{
    if (m_controller) {
        m_controller->setContactOffset(offset);
    }
}

void CharacterController::UpdateTransform()
{
    if (!m_controller) return;

    // ��Ʈ�ѷ��� ��ġ�� Transform ������Ʈ�� �ݿ�
	const PxExtendedVec3& position = m_controller->getFootPosition();

    auto transform = GetTransform();

    transform->SetLocalPosition(XMFLOAT3(
        static_cast<float>(position.x),
        static_cast<float>(position.y + m_groundedOffset),
        static_cast<float>(position.z)
    ));

	// ���� �ý����� ������ ��ġ ���� (TODO: ���� �߰��ϸ� �� ĳ������ ��ġ�θ� �����ϵ��� ����)
    Vec3 pos = transform->GetLocalPosition();

    // forward�� up ���͸� ����ȭ
    Vec3 forward = transform->GetForward();
    Vec3 up = transform->GetUp();

    forward.Normalize();
    up.Normalize();

    // forward�� up ���Ͱ� ������ �ǵ��� ����
    Vec3 right = forward.Cross(up);
	right.Normalize();
    up = right.Cross(forward);
	up.Normalize();

    GET_SINGLE(SoundSystem)->Set3DListenerPosition(pos, forward, up);
}
