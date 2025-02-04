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
    // 캡슐 컨트롤러 생성
    auto transform = GetTransform();
    auto position = transform->GetLocalPosition();

    auto physicsObject = PHYSICS_ENGINE->CreateCapsuleController(
        PxVec3(position.x, position.y, position.z),
        0.5f,   // 반지름
        2.0f,   // 높이
        CollisionGroup::Character,
        CollisionGroup::Default | CollisionGroup::Ground |
        CollisionGroup::Obstacle | CollisionGroup::Character
    );

    if (!physicsObject) {
        Logger::Instance().Error("캐릭터 컨트롤러 생성 실패");
        return;
    }

	// TODO: 캐릭터 컨트롤러 ID 설정
    m_controller = PHYSICS_ENGINE->GetControllerManager()->getController(0);
    Logger::Instance().Debug("캐릭터 컨트롤러 초기화됨");
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

    // 캐릭터 이동 필터 설정
    PxControllerFilters filters;
    PxFilterData filterData(
        static_cast<PxU32>(CollisionGroup::Character),
        static_cast<PxU32>(CollisionGroup::Default | CollisionGroup::Ground | CollisionGroup::Obstacle),
        0, 0);
    filters.mFilterData = &filterData;

    // 이동 수행
    PxControllerCollisionFlags collisionFlags =
        m_controller->move(disp, minDist, deltaTime, filters, obstacleContext);

    // 지면 접촉 상태 갱신 - 비트 연산으로 수정
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

    // 컨트롤러의 위치를 Transform 컴포넌트에 반영
    const PxExtendedVec3& position = m_controller->getPosition();

    auto transform = GetTransform();
    transform->SetLocalPosition(XMFLOAT3(
        static_cast<float>(position.x),
        static_cast<float>(position.y),
        static_cast<float>(position.z)
    ));
}
