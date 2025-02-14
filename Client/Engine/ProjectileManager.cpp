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

    // Ǯ �ʱ�ȭ
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
		Logger::Instance().Warning("Ǯ�� �� �̻� ����ü�� �����ϴ�.");
        return nullptr;
    }

    auto projectileObject = m_projectilePool.front();
    m_projectilePool.pop();

	auto transform = projectileObject->GetTransform();
    transform->SetLocalPosition(position);

    // �Ѿ��� �߻� �������� ���ϵ��� ȸ�� ����
    Vec3 defaultForward = Vec3(0.f, 0.f, 1.f);  // �⺻ ���� ����
    float angle = ::acos(direction.Dot(defaultForward));
    Vec3 rotationAxis = defaultForward.Cross(direction);
    if (rotationAxis.Length() > 0.f) {
        rotationAxis.Normalize();
        angle = rotationAxis.y < 0.f ? -angle : angle;
        transform->SetLocalRotation(Vec3(XM_PIDIV2, angle, 0.f));
    }

	// ���� ����
    auto physicsBody = projectileObject->GetPhysicsBody();
    if (physicsBody) {
        PhysicsBody::CapsuleParams params;
        params.radius = 40.f;
        params.halfHeight = 50.f;

        // �浹 �׷� ����
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

        // PhysX ������ �ڼ��� Transform�� ��ġ��Ŵ
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
    
	Logger::Instance().Info("����ü ����. ID: {}", projectileObject->GetID());
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
		Logger::Instance().Error("���� �����ϴ�.");
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
		Logger::Instance().Info("�ѱ� ��ƼŬ �ý��� ����");
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
			Logger::Instance().Info("����� ��ƼŬ �ý��� {} ����", i + 1);
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
		Logger::Instance().Info("������ �浹 ��ƼŬ �ý��� ����");
    }
}

shared_ptr<GameObject> ProjectileManager::CreateProjectileObject(const Vec3& position, const Vec3& direction)
{
    auto object = make_shared<GameObject>();
    object->AddComponent(make_shared<Transform>());
    object->AddComponent(make_shared<MeshRenderer>());
    object->AddComponent(make_shared<PhysicsBody>());
    object->AddComponent(make_shared<Projectile>());

    // �޽� �� ��Ƽ���� ����
    auto meshRenderer = object->GetMeshRenderer();
    {
        shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadCapsuleMesh(5.f, 50.f);  // radius, halfHeight
        meshRenderer->SetMesh(mesh);
    }
    {
        shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"Laser");
        meshRenderer->SetMaterial(material->Clone());
    }

	Logger::Instance().Info("����ü ����. ID: {}", object->GetID());
    return object;
}

void ProjectileManager::ReturnToPool(shared_ptr<GameObject> projectile)
{
    m_projectilePool.push(projectile);

    if (auto scene = GET_SINGLE(SceneManager)->GetActiveScene()) {
        scene->RemoveGameObject(projectile);
    }

    Logger::Instance().Info("����ü Ǯ�� ��ȯ. ID: {}", projectile->GetID());
}

void ProjectileManager::PlayEffects(const Vec3& position, const Vec3& direction)
{
    // MuzzleFlash ���� �� ���
    auto muzzleObj = m_particleEffects.muzzleFlash;
    muzzleObj->SetCheckFrustum(false);
    auto transform = muzzleObj->GetTransform();
    Vec3 muzzlePos = position + direction * 10.f;
    transform->SetLocalPosition(position);

    // �߻� �������� ȸ��
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
    muzzleDesc.duration = 0.20f;               // ���� �ð�
    muzzleDesc.startScale = 50.f;              // ���� ũ��
    muzzleDesc.endScale = 200.f;               // ���� ũ��
    muzzleDesc.color = Vec4(0.1f, 0.3f, 1.f, 0.7f);  // �Ķ��� �迭
    muzzleDesc.material = GET_SINGLE(Resources)->Get<Material>(L"MuzzleFlash");

    muzzleSystem->Play(muzzleDesc);

    // Shockwave ���� �� ���
    for (int i = 0; i < 3; ++i) {
		auto shockObj = m_particleEffects.shockwaves[i];
		shockObj->SetCheckFrustum(false);

        // �߻� ������ �ٶ󺸵��� ȸ��
        Vec3 shockPos = (position + direction * 10.f) + (direction * 20.f * i);

        auto transform = shockObj->GetTransform();
        transform->SetLocalPosition(shockPos);

        // �߻� �������� ȸ��
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
        shockDesc.startScale = 100.f - (i * 20.f);    // ���� �ٸ� ���� ũ��
        shockDesc.endScale = 10.f;                    // ������ ���� ũ��
        shockDesc.color = Vec4(0.5f, 0.8f, 1.f, 0.7f - (i * 0.15f));  // ���� �����ϰ�
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

    // ī�޶� ���� ����
	auto camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
	if (camera) {
		auto cameraDirection = camera->GetTransform()->GetLook();
        collisionDesc.material->SetVec4(1, 
            Vec4(cameraDirection.x, cameraDirection.y, cameraDirection.z, 0.f));
	}

	m_particleEffects.collisionEffect->GetParticleSystem()->Play(collisionDesc);
}
