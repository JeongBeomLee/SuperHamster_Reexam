#include "pch.h"
#include "Lava.h"
#include "GameObject.h"
#include "Resources.h"
#include "MeshRenderer.h"
#include "Timer.h"

Lava::Lava()
{
}

Lava::~Lava()
{
}

void Lava::Awake()
{
    CreateComponents();
}

void Lava::CreateComponents()
{
    // 메시렌더러 추가
    m_meshRenderer = make_shared<MeshRenderer>();
    GetGameObject()->AddComponent(m_meshRenderer);

    // 사각형 메시 설정
    auto mesh = GET_SINGLE(Resources)->LoadRectangleMesh(900.f, 900.f, 32.f, 32.f);
    m_meshRenderer->SetMesh(mesh);

    // 머터리얼 설정
    m_material = GET_SINGLE(Resources)->Get<Material>(L"Lava")->Clone();
    m_meshRenderer->SetMaterial(m_material);

    // 텍스처 설정
    auto lavaTexture = GET_SINGLE(Resources)->Load<Texture>(
        L"LavaTexture",
        L"..\\Resources\\Texture\\lava_diffuse.png"
    );
    auto normalTexture = GET_SINGLE(Resources)->Load<Texture>(
        L"LavaNormal",
        L"..\\Resources\\Texture\\lava_normal.png"
    );

    m_material->SetTexture(0, lavaTexture);
    m_material->SetTexture(1, normalTexture);

    // 초기 색상 설정
    m_material->SetVec4(0, m_baseColor);
    m_material->SetVec4(1, m_highlightColor);
}

void Lava::Update()
{
    // 시간 업데이트
    m_elapsedTime += DELTA_TIME;

    // 쉐이더 파라미터 업데이트
    m_material->SetFloat(0, m_elapsedTime);     // 경과 시간
    m_material->SetFloat(1, m_waveSpeed);       // 파동 속도
    m_material->SetFloat(2, m_waveStrength);    // 파동 강도
}