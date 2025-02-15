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

    // �ִϸ��̼� ���� ����
    float m_rotationSpeed = XM_PI * 0.5f;  // �ʴ� 90�� ȸ��
    float m_floatSpeed = 2.0f;             // ���� �ӵ�
    float m_floatHeight = 20.0f;            // ���� ����
    float m_elapsedTime = 0.0f;

    // ȹ�� �ִϸ��̼� ���� ����
    bool m_isObtained = false;
    float m_obtainTimer = 0.0f;
    static constexpr float OBTAIN_DURATION = 1.0f;
    Vec3 m_initialScale = Vec3(1.0f, 1.0f, 1.0f);
    Vec3 m_initialPosition;

public:
    // ��ġ ���
    static const std::vector<Vec3> KEY_POSITIONS;
};
