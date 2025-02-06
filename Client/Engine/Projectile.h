#pragma once
#include "MonoBehaviour.h"
#include "PhysicsBody.h"
#include "Material.h"

class Projectile : public MonoBehaviour
{
public:
    Projectile();
    virtual ~Projectile();

    void Initialize(const Vec3& position, const Vec3& direction, float speed = 2000.0f);
    virtual void Update() override;
    void DestroyProjectile();

private:
    Vec3 m_direction;
    float m_speed = 2000.0f;
    float m_lifeTime = 2.0f;
    float m_elapsedTime = 0.0f;

    std::shared_ptr<PhysicsBody> m_physicsBody;
    shared_ptr<Material> m_material;
};