#pragma once
#include "Component.h"
class CharacterController : public Component
{
public:
	CharacterController();
	virtual ~CharacterController();

    void Initialize();
    void Update() override;

    // �̵� ���� �޼���
    void Move(const Vec3& displacement, float deltaTime);
    void ApplyGravity(float deltaTime);
    void SetMoveSpeed(float speed) { m_moveSpeed = speed; }

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

private:
    void UpdateTransform();

private:
    PxController* m_controller = nullptr;
    PxVec3 m_velocity = PxVec3(0.0f);

    // �̵� ���� �Ӽ�
    float m_moveSpeed = 500.0f;      // �̵� �ӵ�
    float m_maxVelocity = 1000.0f;   // �ִ� �ӵ�
    float m_gravity = -981.0f;       // �߷� (�⺻��: -981.0f = -9.81 * 100)
    float m_groundedOffset = 10.0f;   // ���� üũ ������
    bool m_isGrounded = false;       // ���� ���� ����
};

