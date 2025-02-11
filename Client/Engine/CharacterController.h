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

    // �̵� ���� �޼���
    void Move(const Vec3& displacement, float deltaTime);
	void Teleport(const Vec3& position);
    void ApplyGravity(float deltaTime);

    // ���� Ȯ��
    bool IsMoving() const { return m_velocity.magnitude() > 0.001f; }
    bool IsGrounded() const { return m_isGrounded; }

    // �Ӽ� ����
    void SetGravity(float gravity) { m_gravity = gravity; }
    void SetMaxVelocity(float maxVel) { m_maxVelocity = maxVel; }

    // ���� ���� ȹ��
    const PxVec3& GetVelocity() const { return m_velocity; }

    // ���� Ȯ��
    const PxExtendedVec3& GetPosition() const;
    PxController* GetController() const { return m_controller; }

    // ����
    void SetStepOffset(float offset);
    void SetSlopeLimit(float slopeLimit);
    void SetContactOffset(float offset);

    // �浹 �׷� ����
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

    static uint32_t s_controllerId;    // ���� ��Ʈ�ѷ� ID
    uint32_t m_controllerId;           // �� ��Ʈ�ѷ��� ID

    // �̵� ���� �Ӽ�
    float m_maxVelocity = 1000.0f;   // �ִ� �ӵ�
    float m_gravity = -981.0f;       // �߷� (�⺻��: -981.0f = -9.81 * 100)
    float m_groundedOffset = 10.0f;   // ���� üũ ������
    bool m_isGrounded = false;       // ���� ���� ����
};

