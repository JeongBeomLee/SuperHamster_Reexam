#include "pch.h"
#include "Projectile.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Transform.h"
#include "GameObject.h"
#include "Timer.h"
#include "MeshRenderer.h"

Projectile::Projectile()
{
}

Projectile::~Projectile()
{
}

void Projectile::Initialize(const Vec3& direction, float speed)
{
	m_speed = speed;
	m_direction = direction;
	m_elapsedTime = 0.0f;

    if (auto meshRenderer = GetGameObject()->GetMeshRenderer()) {
        m_material = meshRenderer->GetMaterial();
    }

	if (auto physicsBody = GetGameObject()->GetPhysicsBody()) {
        // 초기 속도 설정
        physicsBody->SetLinearVelocity(PxVec3(
            m_direction.x * m_speed,
            0.f,
            m_direction.z * m_speed
        ));
    }
}

void Projectile::Update() 
{
	if (GetGameObject()->IsActive() == false) return;

    // 시간 업데이트
    m_elapsedTime += DELTA_TIME;
    if (m_elapsedTime >= m_lifeTime) {
        DestroyProjectile();
        return;
    }

    // 레이저 이펙트 시간 업데이트
    if (m_material) {
        m_material->SetFloat(0, m_elapsedTime);  // g_float_0에 시간값 전달
    }
}

void Projectile::DestroyProjectile()
{
	GetGameObject()->SetActive(false);
    Logger::Instance().Info("수명이 다한 투사체 삭제. ID: {}", GetGameObject()->GetID());
}
