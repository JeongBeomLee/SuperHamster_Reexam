#include "pch.h"
#include "CharacterController.h"
#include "Transform.h"
#include "Timer.h"
#include "PhysicsTypes.h"
#include "Engine.h"

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
    // 캡슐 컨트롤러 생성
    auto transform = GetTransform();
    auto position = transform->GetLocalPosition();

    auto physicsObject = PHYSICS_ENGINE->CreateCapsuleController(
        PxVec3(position.x, position.y, position.z),
        50.0f,   // 반지름
        150.0f,   // 높이
        CollisionGroup::Character,
        CollisionGroup::Default | CollisionGroup::Ground |
		CollisionGroup::Obstacle | CollisionGroup::Character | 
		CollisionGroup::Enemy | CollisionGroup::Trigger
    );

    if (!physicsObject) {
        Logger::Instance().Error("캐릭터 컨트롤러 생성 실패");
        return;
    }

	// TODO: 캐릭터 컨트롤러 ID 설정
    m_controller = PHYSICS_ENGINE->GetControllerManager()->getController(0);
    m_controller->setUserData(GetGameObject().get());
	m_controller->getActor()->userData = GetGameObject().get();

    Logger::Instance().Debug("캐릭터 컨트롤러 초기화됨");
}

void CharacterController::Update()
{
    if (!m_controller) return;

    float deltaTime = DELTA_TIME;

    // 중력 적용
    ApplyGravity(deltaTime);

    // Transform 컴포넌트 업데이트
    UpdateTransform();
}

void CharacterController::Move(const Vec3& displacement, float deltaTime)
{
    auto obstacleContext = PHYSICS_ENGINE->GetObstacleContext();
    if (!m_controller || !obstacleContext) return;

    PxVec3 disp(displacement.x, displacement.y, displacement.z);

    // 이동 거리 계산
    const float minDist = 0.001f;

    // 캐릭터 이동 필터 설정
    PxControllerFilters filters;
    PxFilterData filterData;
    filterData.word0 = static_cast<PxU32>(CollisionGroup::Character);
    filterData.word1 = static_cast<PxU32>(CollisionGroup::Default |
        CollisionGroup::Ground |
        CollisionGroup::Obstacle);
    filters.mFilterData = &filterData;

    // 이동 수행
    PxControllerCollisionFlags collisionFlags =
        m_controller->move(disp, minDist, deltaTime, filters, obstacleContext);

    // 지면 접촉 상태 업데이트
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

    // 중력 적용
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

    // 컨트롤러의 위치를 Transform 컴포넌트에 반영
	const PxExtendedVec3& position = m_controller->getFootPosition();

    auto transform = GetTransform();
    transform->SetLocalPosition(XMFLOAT3(
        static_cast<float>(position.x),
        static_cast<float>(position.y + m_groundedOffset),
        static_cast<float>(position.z)
    ));
}
