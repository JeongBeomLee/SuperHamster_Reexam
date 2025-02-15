#pragma once
#include "MonoBehaviour.h"

// �Ҳ� ���͸��� �Ķ����
struct FlameParameters {
    Vec4 baseColor = Vec4(1.0f, 0.5f, 0.1f, 1.0f);      // ��Ȳ�� �⺻ ����
    Vec4 tipColor = Vec4(1.0f, 0.8f, 0.3f, 1.0f);       // ����� ���κ� ����
    float flickerSpeed = 0.5f;                           // ������ �ӵ�
    float flickerIntensity = 0.2f;                       // ������ ����
    float verticalSpeed = 2.0f;                          // ���� ������ �ӵ�
    float noiseScale = 3.0f;                            // ������ ������
    float dissolveEdge = 0.1f;                          // ������ ��� �ε巯��
    float glowIntensity = 2.0f;                         // �߱� ����
};

// ���� ���͸��� �Ķ����
struct SmokeParameters {
    Vec4 smokeColor = Vec4(0.2f, 0.2f, 0.2f, 0.9f);     // ���� ����
    float riseSpeed = 0.3f;                              // ��� �ӵ�
    float fadeSpeed = 0.3f;                              // ���̵�ƿ� �ӵ�
    float turbulence = 0.8f;                            // ���� ����
};

class Flame : public MonoBehaviour {
public:
    Flame();
    virtual ~Flame();

    void Awake() override;
    void Update() override;

private:
    void CreateComponents();
    void UpdateFlameEffect();

private:
    // �Ҳ� ������Ʈ
    shared_ptr<GameObject> m_flameObject;
    shared_ptr<MeshRenderer> m_flameRenderer;
    shared_ptr<class Material> m_flameMaterial;

    // ���� ������Ʈ
    shared_ptr<GameObject> m_smokeObject;
    shared_ptr<MeshRenderer> m_smokeRenderer;
    shared_ptr<class Material> m_smokeMaterial;

    // �ִϸ��̼� �Ķ����
    FlameParameters m_params;
    SmokeParameters m_smokeParams;
    float m_elapsedTime = 0.0f;
};