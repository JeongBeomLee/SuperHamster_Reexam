#include "pch.h"
#include "CharacterMovement.h"
#include "Transform.h"
#include "GameObject.h"
#include "Timer.h"

CharacterMovement::CharacterMovement() : Component(COMPONENT_TYPE::CHARACTER_MOVEMENT)
{
}

CharacterMovement::~CharacterMovement()
{
}

void CharacterMovement::Start()
{
    m_characterController = GetGameObject()->GetCharacterController();
    if (!m_characterController) {
        Logger::Instance().Error("CharacterController�� ã�� �� �����ϴ�.");
    }
}

void CharacterMovement::Update()
{
    if (!m_characterController) {
        Logger::Instance().Error("CharacterController�� �����ϴ�.");
        return;
    }

    float deltaTime = DELTA_TIME;
    ProcessMovement(deltaTime);
}

void CharacterMovement::ProcessMovement(float deltaTime)
{
    if (m_moveDirection != Vec3::Zero) {
        UpdateMovement(deltaTime);
    }
}

void CharacterMovement::UpdateMovement(float deltaTime)
{
    Vec3 movementVector = CalculateMovementVector(deltaTime);

    if (movementVector != Vec3::Zero) {
        // �̵� �������� ĳ���� ȸ��
        SmoothRotation(m_moveDirection, deltaTime);

        // CharacterController�� ���� ���� �̵�
        m_characterController->Move(movementVector * m_rollSpeed, deltaTime);
    }
    else {
        // ���� ���� �̵�
		m_characterController->Move(Vec3::Zero, deltaTime);
    }
}

Vec3 CharacterMovement::CalculateMovementVector(float deltaTime)
{
    // ���� ������ ��ǥ �������� �ε巴�� ����
    m_currentDirection = XMVectorLerp(
        m_currentDirection,
        m_moveDirection,
        m_movementSmoothing
    );

    // ���� �̵� ���� ���
    return m_currentDirection * m_moveSpeed * deltaTime;
}

void CharacterMovement::SmoothRotation(const Vec3& targetDirection, float deltaTime)
{
    if (targetDirection == Vec3::Zero) return;

    // ��ǥ ȸ�� ���
    float targetYaw = atan2(-targetDirection.x, -targetDirection.z);

    // ���� ȸ�� ��������
    Vec3 currentRotation = GetTransform()->GetLocalRotation();

    // �ε巯�� ȸ�� ����
    float newYaw = ScalarLerpAngle(
        currentRotation.y,
        targetYaw,
        m_rotationSpeed * deltaTime
    );

    // ���ο� ȸ�� ����
    GetTransform()->SetLocalRotation(Vec3(currentRotation.x, newYaw, currentRotation.z));
}

void CharacterMovement::SetMoveDirection(const Vec3& direction)
{
    m_moveDirection = direction;
    if (m_moveDirection != Vec3::Zero) {
        m_moveDirection.Normalize();
    }
}

void CharacterMovement::StopMovement()
{
    m_moveDirection = Vec3::Zero;
    //m_currentDirection = Vec3::Zero;
}