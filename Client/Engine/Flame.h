#pragma once
#include "MonoBehaviour.h"

// 불꽃 머터리얼 파라미터
struct FlameParameters {
    Vec4 baseColor = Vec4(1.0f, 0.5f, 0.1f, 1.0f);      // 주황색 기본 색상
    Vec4 tipColor = Vec4(1.0f, 0.8f, 0.3f, 1.0f);       // 노란색 끝부분 색상
    float flickerSpeed = 0.5f;                           // 깜빡임 속도
    float flickerIntensity = 0.2f;                       // 깜빡임 강도
    float verticalSpeed = 2.0f;                          // 수직 움직임 속도
    float noiseScale = 3.0f;                            // 노이즈 스케일
    float dissolveEdge = 0.1f;                          // 디졸브 경계 부드러움
    float glowIntensity = 2.0f;                         // 발광 강도
};

// 연기 머터리얼 파라미터
struct SmokeParameters {
    Vec4 smokeColor = Vec4(0.2f, 0.2f, 0.2f, 0.9f);     // 연기 색상
    float riseSpeed = 0.3f;                              // 상승 속도
    float fadeSpeed = 0.3f;                              // 페이드아웃 속도
    float turbulence = 0.8f;                            // 난류 강도
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
    // 불꽃 오브젝트
    shared_ptr<GameObject> m_flameObject;
    shared_ptr<MeshRenderer> m_flameRenderer;
    shared_ptr<class Material> m_flameMaterial;

    // 연기 오브젝트
    shared_ptr<GameObject> m_smokeObject;
    shared_ptr<MeshRenderer> m_smokeRenderer;
    shared_ptr<class Material> m_smokeMaterial;

    // 애니메이션 파라미터
    FlameParameters m_params;
    SmokeParameters m_smokeParams;
    float m_elapsedTime = 0.0f;
};