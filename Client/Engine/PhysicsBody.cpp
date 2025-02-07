#include "pch.h"
#include "PhysicsBody.h"
#include "Transform.h"
#include "Engine.h"

PhysicsBody::PhysicsBody() : Component(COMPONENT_TYPE::PHYSICS_BODY)
{
}

PhysicsBody::~PhysicsBody()
{
    m_physicsObject.reset();
}

void PhysicsBody::Update() {
    if (!m_physicsObject) return;

    if (IsDynamic() || IsKinematic()) {
        UpdatePhysicsTransform();
    }
}

bool PhysicsBody::CreateBody(PhysicsObjectType type,
    PhysicsShapeType shape,
    const BoxParams& params,
    float density) {
    if (shape != PhysicsShapeType::Box) {
        Logger::Instance().Error("�߸��� ���� Ÿ���� ������: Box �ʿ�");
        return false;
    }

    auto transform = GetTransform();
    auto position = transform->GetLocalPosition();
    transform->SetLocalScale(XMFLOAT3(
        params.dimensions.x * 0.05f,
        params.dimensions.y * 0.05f,
        params.dimensions.z * 0.05f));

    m_physicsObject = PHYSICS_ENGINE->CreateBox(
        PxVec3(position.x, position.y, position.z),
        params.dimensions,
        type,
        m_group,
        m_mask,
        density
    );

    if (m_physicsObject) {
        m_shapeType = shape;
        Logger::Instance().Debug("Box ���� ��ü ������");
        return true;
    }

    return false;
}

bool PhysicsBody::CreateBody(PhysicsObjectType type,
    PhysicsShapeType shape,
    const SphereParams& params,
    float density) {
    if (shape != PhysicsShapeType::Sphere) {
        Logger::Instance().Error("�߸��� ���� Ÿ���� ������: Sphere �ʿ�");
        return false;
    }

    auto transform = GetTransform();
    auto position = transform->GetLocalPosition();

    m_physicsObject = PHYSICS_ENGINE->CreateSphere(
        PxVec3(position.x, position.y, position.z),
        params.radius,
        type,
        m_group,
        m_mask,
        density
    );

    if (m_physicsObject) {
        m_shapeType = shape;
        Logger::Instance().Debug("Sphere ���� ��ü ������");
        return true;
    }

    return false;
}

bool PhysicsBody::CreateBody(PhysicsObjectType type,
    PhysicsShapeType shape,
    const CapsuleParams& params,
    float density) {
    if (shape != PhysicsShapeType::Capsule) {
        Logger::Instance().Error("�߸��� ���� Ÿ���� ������: Capsule �ʿ�");
        return false;
    }

    auto transform = GetTransform();
    auto position = transform->GetLocalPosition();

    m_physicsObject = PHYSICS_ENGINE->CreateCapsule(
        PxVec3(position.x, position.y, position.z),
        params.radius,
        params.halfHeight,
        type,
        m_group,
        m_mask,
        density
    );

    if (m_physicsObject) {
        m_shapeType = shape;
        Logger::Instance().Debug("Capsule ���� ��ü ������");
        return true;
    }

    return false;
}

bool PhysicsBody::CreateBody(PhysicsObjectType type,
    PhysicsShapeType shape,
    const MeshParams& params) {
    if (shape != PhysicsShapeType::TriangleMesh) {
        Logger::Instance().Error("�߸��� ���� Ÿ���� ������: TriangleMesh �ʿ�");
        return false;
    }

    auto transform = GetTransform();
    auto position = transform->GetLocalPosition();

    m_physicsObject = PHYSICS_ENGINE->CreateTriangleMesh(
        PxVec3(position.x, position.y, position.z),
        params.vertices,
        params.indices,
        type,
        m_group,
        m_mask
    );

    if (m_physicsObject) {
        m_shapeType = shape;
        Logger::Instance().Debug("TriangleMesh ���� ��ü ������");
        return true;
    }

    return false;
}

void PhysicsBody::UpdatePhysicsTransform() {
    if (!m_physicsObject) return;

    // PhysX�� ��ȯ ����� GameObject�� Transform�� ����
    auto worldMatrix = m_physicsObject->GetTransformMatrix();

    // ��ǥ�� ��ȯ�� ���� ȸ�� ��� ���� (PhysX Z-up -> DirectX Y-up)
    XMMATRIX coordinateConversion = XMMatrixRotationRollPitchYaw(XM_PIDIV2, 0.f, 0.f);

    // ��ȯ ��Ŀ� ��ǥ�� ��ȯ ����
    worldMatrix = XMMatrixMultiply(coordinateConversion, worldMatrix);

    // ��ġ, ȸ��, ũ�� ����
    XMVECTOR scale, rotationQuat, translation;
    XMMatrixDecompose(&scale, &rotationQuat, &translation, worldMatrix);

    // Transform ������Ʈ ������Ʈ
    auto transform = GetTransform();

    // ��ġ ����
    XMFLOAT3 position;
    XMStoreFloat3(&position, translation);
	transform->SetLocalPosition(position);

    // ȸ�� ���� - ���ʹϿ��� ���Ϸ� ������ ��ȯ
    /*XMFLOAT3 rotationEuler;
    QuaternionToEulerAngles(rotationQuat, &rotationEuler);*/

    // ���Ϸ� ���� ������ ��ȯ
    /*rotationEuler.x = XMConvertToDegrees(rotationEuler.x + XM_PIDIV2);
    rotationEuler.y = XMConvertToDegrees(rotationEuler.y);
    rotationEuler.z = XMConvertToDegrees(rotationEuler.z);

    transform->SetLocalRotation(rotationEuler);*/

    XMFLOAT3 scaleFloat;
    XMStoreFloat3(&scaleFloat, scale);
    transform->SetLocalScale(scaleFloat);
}

void PhysicsBody::AddForce(const PxVec3& force, PxForceMode::Enum mode) {
    if (!m_physicsObject || !IsDynamic()) return;

    auto actor = static_cast<PxRigidDynamic*>(m_physicsObject->GetActor());
    actor->addForce(force, mode);
}

void PhysicsBody::AddTorque(const PxVec3& torque, PxForceMode::Enum mode) {
    if (!m_physicsObject || !IsDynamic()) return;

    auto actor = static_cast<PxRigidDynamic*>(m_physicsObject->GetActor());
    actor->addTorque(torque, mode);
}

void PhysicsBody::SetLinearVelocity(const PxVec3& velocity) {
    if (!m_physicsObject || !IsDynamic()) return;

    auto actor = static_cast<PxRigidDynamic*>(m_physicsObject->GetActor());
    actor->setLinearVelocity(velocity);
}

void PhysicsBody::SetAngularVelocity(const PxVec3& velocity) {
    if (!m_physicsObject || !IsDynamic()) return;

    auto actor = static_cast<PxRigidDynamic*>(m_physicsObject->GetActor());
    actor->setAngularVelocity(velocity);
}

bool PhysicsBody::IsKinematic() const {
    if (!m_physicsObject) return false;
    auto actor = m_physicsObject->GetActor();
    return actor->is<PxRigidDynamic>() && static_cast<PxRigidDynamic*>(actor)->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC;
}

bool PhysicsBody::IsDynamic() const {
    if (!m_physicsObject) return false;
    return m_physicsObject->GetActor()->is<PxRigidDynamic>();
}

bool PhysicsBody::IsStatic() const {
    if (!m_physicsObject) return false;
    return m_physicsObject->GetActor()->is<PxRigidStatic>();
}