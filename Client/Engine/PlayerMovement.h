#pragma once
#include "Component.h"
#include "CharacterController.h"

class PlayerMovement : public Component
{
public:
    PlayerMovement();
    virtual ~PlayerMovement();

    virtual void Start() override;
    virtual void Update() override;

    void ProcessMovement(float deltaTime);
    void SetMoveDirection(const Vec3& direction);
    void StopMovement();

    const Vec3& GetMoveDirection() const { return m_moveDirection; }
    float GetMoveSpeed() const { return m_moveSpeed; }

private:
    void UpdateMovement(float deltaTime);
    Vec3 CalculateMovementVector(float deltaTime);
    void SmoothRotation(const Vec3& targetDirection, float deltaTime);

private:
    std::shared_ptr<CharacterController> m_characterController;
    Vec3 m_moveDirection = Vec3::Zero;
    Vec3 m_currentDirection = Vec3::Zero;

    float m_moveSpeed = 500.0f;
    float m_rotationSpeed = 10.0f;
    float m_movementSmoothing = 0.1f;
};