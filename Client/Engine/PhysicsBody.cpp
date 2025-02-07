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
        Logger::Instance().Error("잘못된 형상 타입이 지정됨: Box 필요");
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
        Logger::Instance().Debug("Box 물리 객체 생성됨");
        return true;
    }

    return false;
}

bool PhysicsBody::CreateBody(PhysicsObjectType type,
    PhysicsShapeType shape,
    const SphereParams& params,
    float density) {
    if (shape != PhysicsShapeType::Sphere) {
        Logger::Instance().Error("잘못된 형상 타입이 지정됨: Sphere 필요");
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
        Logger::Instance().Debug("Sphere 물리 객체 생성됨");
        return true;
    }

    return false;
}

bool PhysicsBody::CreateBody(PhysicsObjectType type,
    PhysicsShapeType shape,
    const CapsuleParams& params,
    float density) {
    if (shape != PhysicsShapeType::Capsule) {
        Logger::Instance().Error("잘못된 형상 타입이 지정됨: Capsule 필요");
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
        Logger::Instance().Debug("Capsule 물리 객체 생성됨");
        return true;
    }

    return false;
}

bool PhysicsBody::CreateBody(PhysicsObjectType type,
    PhysicsShapeType shape,
    const MeshParams& params) {
    if (shape != PhysicsShapeType::TriangleMesh) {
        Logger::Instance().Error("잘못된 형상 타입이 지정됨: TriangleMesh 필요");
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
        Logger::Instance().Debug("TriangleMesh 물리 객체 생성됨");
        return true;
    }

    return false;
}

void PhysicsBody::UpdatePhysicsTransform() {
    if (!m_physicsObject) return;

    // PhysX의 변환 행렬을 GameObject의 Transform에 적용
    auto worldMatrix = m_physicsObject->GetTransformMatrix();

    // 좌표계 변환을 위한 회전 행렬 생성 (PhysX Z-up -> DirectX Y-up)
    XMMATRIX coordinateConversion = XMMatrixRotationRollPitchYaw(XM_PIDIV2, 0.f, 0.f);

    // 변환 행렬에 좌표계 변환 적용
    worldMatrix = XMMatrixMultiply(coordinateConversion, worldMatrix);

    // 위치, 회전, 크기 추출
    XMVECTOR scale, rotationQuat, translation;
    XMMatrixDecompose(&scale, &rotationQuat, &translation, worldMatrix);

    // Transform 컴포넌트 업데이트
    auto transform = GetTransform();

    // 위치 설정
    XMFLOAT3 position;
    XMStoreFloat3(&position, translation);
	transform->SetLocalPosition(position);

    // 회전 설정 - 쿼터니온을 오일러 각으로 변환
    /*XMFLOAT3 rotationEuler;
    QuaternionToEulerAngles(rotationQuat, &rotationEuler);*/

    // 오일러 각을 각도로 변환
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