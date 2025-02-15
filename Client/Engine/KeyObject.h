#pragma once
#include "MonoBehaviour.h"
#include "EventManager.h"

class KeyObject : public MonoBehaviour {
public:
    KeyObject();
    virtual ~KeyObject();

    void Awake() override;
    void Start() override;
    void Update() override;

    void OnTriggerEnter(const Event::TriggerEvent& event);

private:
    void CreateKeyObject();
    void UpdateFloatingAnimation();
    void UpdateObtainAnimation();

private:
    std::shared_ptr<GameObject> m_keyObject;
    static int s_keyId;
    int m_keyId = 0;
    size_t m_triggerEventId;

    // 애니메이션 관련 변수
    float m_rotationSpeed = XM_PI * 0.5f;  // 초당 90도 회전
    float m_floatSpeed = 2.0f;             // 부유 속도
    float m_floatHeight = 20.0f;            // 부유 높이
    float m_elapsedTime = 0.0f;

    // 획득 애니메이션 관련 변수
    bool m_isObtained = false;
    float m_obtainTimer = 0.0f;
    static constexpr float OBTAIN_DURATION = 1.0f;
    Vec3 m_initialScale = Vec3(1.0f, 1.0f, 1.0f);
    Vec3 m_initialPosition;

public:
    // 위치 상수
    static const std::vector<Vec3> KEY_POSITIONS;
};
