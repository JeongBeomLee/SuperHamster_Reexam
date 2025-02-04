#pragma once
#include "Component.h"
#include "PhysicsTypes.h"
#include "PhysicsObject.h"

class PhysicsBody : public Component {
public:
    struct BoxParams {
        PxVec3 dimensions = PxVec3(1.0f);
    };

    struct SphereParams {
        float radius = 1.0f;
    };

    struct CapsuleParams {
        float radius = 0.5f;
        float halfHeight = 1.0f;
    };

    struct MeshParams {
        std::vector<PxVec3> vertices;
        std::vector<uint32_t> indices;
    };

	PhysicsBody();
	~PhysicsBody() override;

public:
	void Update() override;

    // ���� ��ü ���� �޼��� (�ڽ�, ��, ĸ��, �޽�)
    bool CreateBody(PhysicsObjectType type,
        PhysicsShapeType shape,
        const BoxParams& params,    // �ڽ� ����
        float density = 1.0f);

    bool CreateBody(PhysicsObjectType type,
        PhysicsShapeType shape,
        const SphereParams& params, // �� ����
        float density = 1.0f);

    bool CreateBody(PhysicsObjectType type,
        PhysicsShapeType shape,
        const CapsuleParams& params,// ĸ�� ����
        float density = 1.0f);

    bool CreateBody(PhysicsObjectType type,
        PhysicsShapeType shape,
        const MeshParams& params); // �޽� ����

    // ���� ��ü ���� �޼���
    void AddForce(const PxVec3& force, PxForceMode::Enum mode = PxForceMode::eFORCE);
    void AddTorque(const PxVec3& torque, PxForceMode::Enum mode = PxForceMode::eFORCE);
    void SetLinearVelocity(const PxVec3& velocity);
    void SetAngularVelocity(const PxVec3& velocity);

    // �浹 �׷� ����
    void SetCollisionGroup(CollisionGroup group) { m_group = group; }
    void SetCollisionMask(CollisionGroup mask) { m_mask = mask; }

    // ���� ��ü ���� Ȯ��
    bool IsKinematic() const;
    bool IsDynamic() const;
    bool IsStatic() const;

private:
    void UpdatePhysicsTransform();  // ���� ������ ��ȯ ����� �����ͼ� Transform ������Ʈ

    std::shared_ptr<PhysicsObject> m_physicsObject;
    CollisionGroup m_group = CollisionGroup::Default;
    CollisionGroup m_mask = CollisionGroup::Default;
    PhysicsShapeType m_shapeType;
};

