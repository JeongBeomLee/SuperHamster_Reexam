#pragma once
#include "GameObject.h"

class ProjectileManager
{
    DECLARE_SINGLE(ProjectileManager);

public:
    void Initialize(uint32_t poolSize = 50);
    shared_ptr<GameObject> SpawnProjectile(const Vec3& position, const Vec3& direction);
    void Update();

private:
    void InitializeParticleEffects();
    shared_ptr<GameObject> CreateProjectileObject(const Vec3& position, const Vec3& direction);
    void ReturnToPool(shared_ptr<GameObject> projectile);

private:
    void PlayEffects(const Vec3& position, const Vec3& direction);

public:
	void PlayCollisionEffect(const Vec3& position);

public:
    static constexpr float PROJECTILE_DAMAGE = 20.0f;
private:
    struct ParticleEffects {
        shared_ptr<GameObject> muzzleFlash;
        array<shared_ptr<GameObject>, 3> shockwaves;  // 3개의 충격파
        shared_ptr<GameObject> collisionEffect;
    };

	ParticleEffects m_particleEffects;
    queue<shared_ptr<GameObject>> m_projectilePool;
    vector<shared_ptr<GameObject>> m_activeProjectiles;
    uint32_t m_poolSize = 50;
};