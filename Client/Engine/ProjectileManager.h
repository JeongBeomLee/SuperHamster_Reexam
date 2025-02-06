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
    shared_ptr<GameObject> CreateProjectileObject(const Vec3& position, const Vec3& direction);
    void ReturnToPool(shared_ptr<GameObject> projectile);

private:
    queue<shared_ptr<GameObject>> m_projectilePool;
    vector<shared_ptr<GameObject>> m_activeProjectiles;
    uint32_t m_poolSize = 50;
};