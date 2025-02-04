#pragma once
#include "Component.h"
class CharaterController : public Component
{
public:
	CharaterController();
	virtual ~CharaterController();

    void Initialize();
    void Update() override;

    // 캐릭터 이동 제어 메서드
    void Move(const XMFLOAT3& displacement, float minDist, float deltaTime);

    // 상태 확인
    bool IsOnGround() const { return m_isOnGround; }
    const PxExtendedVec3& GetPosition() const;
    PxController* GetController() const { return m_controller; }

    // 설정
    void SetStepOffset(float offset);
    void SetSlopeLimit(float slopeLimit);
    void SetContactOffset(float offset);

private:
    void UpdateTransform();
    //void HandleInput(float deltaTime);

    PxController* m_controller = nullptr;
    bool m_isOnGround = true;
    PxVec3 m_velocity = PxVec3(0.0f);
    float m_moveSpeed = 5.0f;
    float m_jumpForce = 10.0f;
    float m_gravity = -9.81f;
};

