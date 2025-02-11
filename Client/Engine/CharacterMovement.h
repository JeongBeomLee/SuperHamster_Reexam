#pragma once
#include "Component.h"
#include "CharacterController.h"

class CharacterMovement : public Component
{
public:
    CharacterMovement();
    virtual ~CharacterMovement();

    virtual void Start() override;
    virtual void Update() override;

    void ProcessMovement(float deltaTime);
    void SetMoveDirection(const Vec3& direction);
    void StopMovement();
    void SmoothRotation(const Vec3& targetDirection, float deltaTime);

    const Vec3& GetMoveDirection() const { return m_moveDirection; }
	const Vec3& GetCurrentDirection() const { return m_currentDirection; }
    float GetMoveSpeed() const { return m_moveSpeed; }
    
	void SetRollSpeed(float speed) { m_rollSpeed = speed; }

private:
    void UpdateMovement(float deltaTime);
    Vec3 CalculateMovementVector(float deltaTime);

private:
    std::shared_ptr<CharacterController> m_characterController;
    Vec3 m_moveDirection = Vec3::Zero;
    Vec3 m_currentDirection = Vec3::Zero;

    float m_moveSpeed = 500.0f;
    float m_rotationSpeed = 10.0f;
    float m_rollSpeed = 1.0f;   // 구르기 기능이 있는 캐릭터만 사용
    float m_movementSmoothing = 0.1f;
};