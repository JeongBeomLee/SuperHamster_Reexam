#include "pch.h"
#include "ProjectileManager.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "PhysicsBody.h"
#include "Resources.h"
#include "Projectile.h"
#include "Material.h"
#include "Scene.h"
#include "SceneManager.h"

void ProjectileManager::Initialize(uint32_t poolSize)
{
    m_poolSize = poolSize;

    // 풀 초기화
    for (uint32_t i = 0; i < m_poolSize; ++i) {
        auto projectile = CreateProjectileObject(Vec3::Zero, Vec3::Forward);
        projectile->SetActive(false);
        m_projectilePool.push(projectile);
    }
}

shared_ptr<GameObject> ProjectileManager::SpawnProjectile(const Vec3& position, const Vec3& direction)
{
    if (m_projectilePool.empty()) {
		Logger::Instance().Warning("풀에 더 이상 투사체가 없습니다.");
        return nullptr;
    }

    auto projectileObject = m_projectilePool.front();
    m_projectilePool.pop();

	auto transform = projectileObject->GetTransform();
    transform->SetLocalPosition(position);

    // 총알이 발사 방향으로 향하도록 회전 설정
    Vec3 defaultForward = Vec3(0.f, 0.f, 1.f);  // 기본 전방 벡터
    float angle = ::acos(direction.Dot(defaultForward));
    Vec3 rotationAxis = defaultForward.Cross(direction);
    if (rotationAxis.Length() > 0.f) {
        rotationAxis.Normalize();
        angle = rotationAxis.y < 0.f ? -angle : angle;
        transform->SetLocalRotation(Vec3(XM_PIDIV2, angle, 0.f));
    }

	// 물리 설정
    auto physicsBody = projectileObject->GetPhysicsBody();
    if (physicsBody) {
        PhysicsBody::CapsuleParams params;
        params.radius = 10.f;
        params.halfHeight = 25.f;

        // 충돌 그룹 설정
        physicsBody->SetCollisionGroup(CollisionGroup::Projectile);
        physicsBody->SetCollisionMask(
            CollisionGroup::Default |
            CollisionGroup::Ground |
            CollisionGroup::Enemy |
            CollisionGroup::Obstacle
        );

        physicsBody->CreateBody(
            PhysicsObjectType::DYNAMIC,
            PhysicsShapeType::Capsule,
            params,
            1.0f  // density
        );

        // PhysX 액터의 자세를 Transform과 일치시킴
        auto actor = physicsBody->GetPhysicsObject()->GetActor();
        if (actor) {
            PxTransform currentPose = actor->getGlobalPose();
            float rotY = transform->GetLocalRotation().y + XM_PIDIV2;
            PxQuat pxQuat(rotY, PxVec3(0, 1, 0));
            PxTransform newPose(currentPose.p, pxQuat);
            actor->setGlobalPose(newPose);
			actor->setName("Projectile");
            actor->userData = projectileObject.get();
        }
    }

    auto projectile = projectileObject->GetMonoBehaviour<Projectile>();
    if (projectile) {
        projectile->Initialize(direction);
    }

	projectileObject->SetActive(true);
	m_activeProjectiles.push_back(projectileObject);

    if (auto scene = GET_SINGLE(SceneManager)->GetActiveScene()) {
        scene->AddGameObject(projectileObject);
    }
    
	Logger::Instance().Info("투사체 스폰. ID: {}", projectileObject->GetID());
    return projectileObject;
}

void ProjectileManager::Update()
{
    if (!m_activeProjectiles.empty()) {
        auto it = m_activeProjectiles.begin();
        while (it != m_activeProjectiles.end()) {
            auto projectile = *it;
            if (!projectile->IsActive()) {
                ReturnToPool(projectile);
                it = m_activeProjectiles.erase(it);
            }
            else {
                ++it;
            }
        }
    }
}

shared_ptr<GameObject> ProjectileManager::CreateProjectileObject(const Vec3& position, const Vec3& direction)
{
    auto object = make_shared<GameObject>();
    object->AddComponent(make_shared<Transform>());
    object->AddComponent(make_shared<MeshRenderer>());
    object->AddComponent(make_shared<PhysicsBody>());
    object->AddComponent(make_shared<Projectile>());

    // 메시 및 머티리얼 설정
    auto meshRenderer = object->GetMeshRenderer();
    {
        shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadCapsuleMesh(10.f, 50.f);  // radius, halfHeight
        meshRenderer->SetMesh(mesh);
    }
    {
        shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"Laser");
        meshRenderer->SetMaterial(material->Clone());
    }

	Logger::Instance().Info("투사체 생성. ID: {}", object->GetID());
    return object;
}

void ProjectileManager::ReturnToPool(shared_ptr<GameObject> projectile)
{
    m_projectilePool.push(projectile);

    if (auto scene = GET_SINGLE(SceneManager)->GetActiveScene()) {
        scene->RemoveGameObject(projectile);
    }

    Logger::Instance().Info("투사체 풀로 반환. ID: {}", projectile->GetID());
}
