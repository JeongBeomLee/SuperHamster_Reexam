#include "pch.h"
#include "PlayerMovement.h"
#include "Transform.h"
#include "GameObject.h"
#include "Timer.h"

PlayerMovement::PlayerMovement() : Component(COMPONENT_TYPE::PLAYER_MOVEMENT)
{
}

PlayerMovement::~PlayerMovement()
{
}

void PlayerMovement::Start()
{
    m_characterController = GetGameObject()->GetCharacterController();
    if (!m_characterController) {
        Logger::Instance().Error("CharacterController를 찾을 수 없습니다.");
    }
}

void PlayerMovement::Update()
{
    if (!m_characterController) return;

    float deltaTime = DELTA_TIME;
    ProcessMovement(deltaTime);
}

void PlayerMovement::ProcessMovement(float deltaTime)
{
    if (m_moveDirection != Vec3::Zero) {
        UpdateMovement(deltaTime);
    }
}

void PlayerMovement::UpdateMovement(float deltaTime)
{
    Vec3 movementVector = CalculateMovementVector(deltaTime);

    if (movementVector != Vec3::Zero) {
        // 이동 방향으로 캐릭터 회전
        SmoothRotation(m_moveDirection, deltaTime);

        // CharacterController를 통한 실제 이동
        m_characterController->Move(movementVector, deltaTime);
    }
}

Vec3 PlayerMovement::CalculateMovementVector(float deltaTime)
{
    // 현재 방향을 목표 방향으로 부드럽게 보간
    m_currentDirection = XMVectorLerp(
        m_currentDirection,
        m_moveDirection,
        m_movementSmoothing
    );

    // 최종 이동 벡터 계산
    return m_currentDirection * m_moveSpeed * deltaTime;
}

void PlayerMovement::SmoothRotation(const Vec3& targetDirection, float deltaTime)
{
    if (targetDirection == Vec3::Zero) return;

    // 목표 회전 계산
    float targetYaw = atan2(-targetDirection.x, -targetDirection.z);

    // 현재 회전 가져오기
    Vec3 currentRotation = GetTransform()->GetLocalRotation();

    // 부드러운 회전 보간
    float newYaw = ScalarLerpAngle(
        currentRotation.y,
        targetYaw,
        m_rotationSpeed * deltaTime
    );

    // 새로운 회전 적용
    GetTransform()->SetLocalRotation(Vec3(currentRotation.x, newYaw, currentRotation.z));
}

void PlayerMovement::SetMoveDirection(const Vec3& direction)
{
    m_moveDirection = direction;
    if (m_moveDirection != Vec3::Zero) {
        m_moveDirection.Normalize();
    }
}

void PlayerMovement::StopMovement()
{
    m_moveDirection = Vec3::Zero;
    m_currentDirection = Vec3::Zero;
}