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
    // 캡슐 컨트롤러 생성
    auto transform = GetTransform();
    auto position = transform->GetLocalPosition();

    auto physicsObject = PHYSICS_ENGINE->CreateCapsuleController(
        PxVec3(position.x, position.y, position.z),
        40.0f,   // 반지름
        150.0f,   // 높이
        CollisionGroup::Player,
        CollisionGroup::Default | CollisionGroup::Ground |
		CollisionGroup::Obstacle | CollisionGroup::Player | 
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
    filterData.word0 = static_cast<PxU32>(CollisionGroup::Player);
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

	// 사운드 시스템의 리스너 위치 설정 (TODO: 서버 추가하면 내 캐릭터의 위치로만 설정하도록 수정)
    Vec3 pos = transform->GetLocalPosition();

    // forward와 up 벡터를 정규화
    Vec3 forward = transform->GetForward();
    Vec3 up = transform->GetUp();

    forward.Normalize();
    up.Normalize();

    // forward와 up 벡터가 수직이 되도록 조정
    Vec3 right = forward.Cross(up);
	right.Normalize();
    up = right.Cross(forward);
	up.Normalize();

    GET_SINGLE(SoundSystem)->Set3DListenerPosition(pos, forward, up);
}
