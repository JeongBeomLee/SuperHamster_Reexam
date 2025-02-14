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
#include "ParticleSystem.h"
#include "Camera.h"

void ProjectileManager::Initialize(uint32_t poolSize)
{
    m_poolSize = poolSize;

    // 풀 초기화
    for (uint32_t i = 0; i < m_poolSize; ++i) {
        auto projectile = CreateProjectileObject(Vec3::Zero, Vec3::Forward);
		projectile->SetName(L"Projectile" + to_wstring(i + 1));
        projectile->SetActive(false);
        m_projectilePool.push(projectile);
    }

    InitializeParticleEffects();
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
        params.radius = 40.f;
        params.halfHeight = 50.f;

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
        projectile->Initialize(position, direction);
    }

	projectileObject->SetActive(true);
	m_activeProjectiles.push_back(projectileObject);

    if (auto scene = GET_SINGLE(SceneManager)->GetActiveScene()) {
        scene->AddGameObject(projectileObject);
    }

    PlayEffects(position, direction);
    
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

void ProjectileManager::InitializeParticleEffects()
{
	auto scene = GET_SINGLE(SceneManager)->GetActiveScene();
    if (!scene) {
		Logger::Instance().Error("씬이 없습니다.");
		return;
    }

    // muzzle falsh
    {
		m_particleEffects.muzzleFlash = make_shared<GameObject>();
		m_particleEffects.muzzleFlash->SetName(L"MuzzleFlash");
		m_particleEffects.muzzleFlash->SetCheckFrustum(false);
        m_particleEffects.muzzleFlash->AddComponent(make_shared<Transform>());
        auto muzzleParticle = make_shared<ParticleSystem>();
        m_particleEffects.muzzleFlash->AddComponent(muzzleParticle);
		scene->AddGameObject(m_particleEffects.muzzleFlash);
		Logger::Instance().Info("총구 파티클 시스템 생성");
    }

    // shockwaves
    {
        for (int i = 0; i < 3; ++i) {
			m_particleEffects.shockwaves[i] = make_shared<GameObject>();
			m_particleEffects.shockwaves[i]->SetName(L"Shockwave" + to_wstring(i + 1));
			m_particleEffects.shockwaves[i]->SetCheckFrustum(false);
			m_particleEffects.shockwaves[i]->AddComponent(make_shared<Transform>());
			auto shockwaveParticle = make_shared<ParticleSystem>();
			m_particleEffects.shockwaves[i]->AddComponent(shockwaveParticle);
			scene->AddGameObject(m_particleEffects.shockwaves[i]);
			Logger::Instance().Info("충격파 파티클 시스템 {} 생성", i + 1);
        }
    }

    // laserImpact
    {
		m_particleEffects.collisionEffect = make_shared<GameObject>();
        m_particleEffects.collisionEffect->SetName(L"CollisionEffect");
		m_particleEffects.collisionEffect->SetCheckFrustum(false);
		m_particleEffects.collisionEffect->AddComponent(make_shared<Transform>());
		auto collisonEffectParticle = make_shared<ParticleSystem>();
		m_particleEffects.collisionEffect->AddComponent(collisonEffectParticle);
		scene->AddGameObject(m_particleEffects.collisionEffect);
		Logger::Instance().Info("레이저 충돌 파티클 시스템 생성");
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
        shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadCapsuleMesh(5.f, 50.f);  // radius, halfHeight
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

void ProjectileManager::PlayEffects(const Vec3& position, const Vec3& direction)
{
    // MuzzleFlash 설정 및 재생
    auto muzzleObj = m_particleEffects.muzzleFlash;
    muzzleObj->SetCheckFrustum(false);
    auto transform = muzzleObj->GetTransform();
    Vec3 muzzlePos = position + direction * 10.f;
    transform->SetLocalPosition(position);

    // 발사 방향으로 회전
    Vec3 defaultForward = Vec3(0.f, 0.f, 1.f);
    float angle = ::acos(direction.Dot(defaultForward));
    Vec3 rotationAxis = defaultForward.Cross(direction);
    if (rotationAxis.Length() > 0.f) {
        rotationAxis.Normalize();
        angle = rotationAxis.y < 0.f ? -angle : angle;
        transform->SetLocalRotation(Vec3(XM_PIDIV2, angle, 0.f));
    }

	auto muzzleSystem = muzzleObj->GetParticleSystem();

    ParticleSystem::EffectDesc muzzleDesc;
    muzzleDesc.type = ParticleSystem::EffectType::MUZZLE_FLASH;
    muzzleDesc.duration = 0.20f;               // 지속 시간
    muzzleDesc.startScale = 50.f;              // 시작 크기
    muzzleDesc.endScale = 200.f;               // 종료 크기
    muzzleDesc.color = Vec4(0.1f, 0.3f, 1.f, 0.7f);  // 파란색 계열
    muzzleDesc.material = GET_SINGLE(Resources)->Get<Material>(L"MuzzleFlash");

    muzzleSystem->Play(muzzleDesc);

    // Shockwave 설정 및 재생
    for (int i = 0; i < 3; ++i) {
		auto shockObj = m_particleEffects.shockwaves[i];
		shockObj->SetCheckFrustum(false);

        // 발사 방향을 바라보도록 회전
        Vec3 shockPos = (position + direction * 10.f) + (direction * 20.f * i);

        auto transform = shockObj->GetTransform();
        transform->SetLocalPosition(shockPos);

        // 발사 방향으로 회전
        Vec3 defaultForward = Vec3(0.f, 0.f, 1.f);
        float angle = ::acos(direction.Dot(defaultForward));
        Vec3 rotationAxis = defaultForward.Cross(direction);
        if (rotationAxis.Length() > 0.f) {
            rotationAxis.Normalize();
            angle = rotationAxis.y < 0.f ? -angle : angle;
            transform->SetLocalRotation(Vec3(0.f, angle, 0.f));
        }

        auto shockSystem = shockObj->GetParticleSystem();

        ParticleSystem::EffectDesc shockDesc;
        shockDesc.type = ParticleSystem::EffectType::SHOCKWAVE;
        shockDesc.duration = 0.3f;
        shockDesc.startScale = 100.f - (i * 20.f);    // 각각 다른 시작 크기
        shockDesc.endScale = 10.f;                    // 동일한 종료 크기
        shockDesc.color = Vec4(0.5f, 0.8f, 1.f, 0.7f - (i * 0.15f));  // 점점 투명하게
        shockDesc.material = GET_SINGLE(Resources)->Get<Material>(L"Shockwave");

        shockSystem->Play(shockDesc);
    }
}

void ProjectileManager::PlayCollisionEffect(const Vec3& position)
{
	m_particleEffects.collisionEffect->GetTransform()->SetLocalPosition(position);

    ParticleSystem::EffectDesc collisionDesc;
    collisionDesc.type = ParticleSystem::EffectType::COLLISION;
    collisionDesc.duration = 0.2f;
    collisionDesc.startScale = 50.f;
    collisionDesc.endScale = 150.f;
    collisionDesc.color = Vec4(0.5f, 0.8f, 1.f, 1.f);
    collisionDesc.material = GET_SINGLE(Resources)->Get<Material>(L"CollisionEffect");

    // 카메라 방향 전달
	auto camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
	if (camera) {
		auto cameraDirection = camera->GetTransform()->GetLook();
        collisionDesc.material->SetVec4(1, 
            Vec4(cameraDirection.x, cameraDirection.y, cameraDirection.z, 0.f));
	}

	m_particleEffects.collisionEffect->GetParticleSystem()->Play(collisionDesc);
}
