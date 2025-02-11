#pragma once
#include "Component.h"
#include "PhysicsTypes.h"

class CharacterController : public Component
{
public:
	CharacterController();
	virtual ~CharacterController();

    void Initialize();
    void Update() override;

    // 이동 관련 메서드
    void Move(const Vec3& displacement, float deltaTime);
	void Teleport(const Vec3& position);
    void ApplyGravity(float deltaTime);

    // 상태 확인
    bool IsMoving() const { return m_velocity.magnitude() > 0.001f; }
    bool IsGrounded() const { return m_isGrounded; }

    // 속성 설정
    void SetGravity(float gravity) { m_gravity = gravity; }
    void SetMaxVelocity(float maxVel) { m_maxVelocity = maxVel; }

    // 현재 상태 획득
    const PxVec3& GetVelocity() const { return m_velocity; }

    // 상태 확인
    const PxExtendedVec3& GetPosition() const;
    PxController* GetController() const { return m_controller; }

    // 설정
    void SetStepOffset(float offset);
    void SetSlopeLimit(float slopeLimit);
    void SetContactOffset(float offset);

    // 충돌 그룹 설정
    CollisionGroup GetCollisionGroup() const { return m_group; }
    CollisionGroup GetCollisionMask() const { return m_mask; }
    void SetCollisionGroup(CollisionGroup group) { m_group = group; }
    void SetCollisionMask(CollisionGroup mask) { m_mask = mask; }

private:
    void UpdateTransform();

private:
    PxController* m_controller = nullptr;
    PxVec3 m_velocity = PxVec3(0.0f);
    CollisionGroup m_group = CollisionGroup::Default;
    CollisionGroup m_mask = CollisionGroup::Default;

    static uint32_t s_controllerId;    // 정적 컨트롤러 ID
    uint32_t m_controllerId;           // 이 컨트롤러의 ID

    // 이동 관련 속성
    float m_maxVelocity = 1000.0f;   // 최대 속도
    float m_gravity = -981.0f;       // 중력 (기본값: -981.0f = -9.81 * 100)
    float m_groundedOffset = 10.0f;   // 지면 체크 오프셋
    bool m_isGrounded = false;       // 지면 접촉 상태
};

