#pragma once
#include "MonoBehaviour.h"
class Lava : public MonoBehaviour {
public:
    Lava();
    virtual ~Lava();

    void Awake() override;
    void Update() override;

private:
    void CreateComponents();

private:
    shared_ptr<MeshRenderer> m_meshRenderer;
    shared_ptr<class Material> m_material;

    float m_elapsedTime = 0.0f;

    // �ִϸ��̼� �Ķ����
    Vec4 m_baseColor = Vec4(0.8f, 0.2f, 0.0f, 1.0f);    // ������
    Vec4 m_highlightColor = Vec4(1.0f, 0.6f, 0.0f, 1.0f); // ���� ��Ȳ��
    float m_waveSpeed = 2.0f;
    float m_waveStrength = 0.02f;
};