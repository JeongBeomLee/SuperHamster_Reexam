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
    // �޽÷����� �߰�
    m_meshRenderer = make_shared<MeshRenderer>();
    GetGameObject()->AddComponent(m_meshRenderer);

    // �簢�� �޽� ����
    auto mesh = GET_SINGLE(Resources)->LoadRectangleMesh(900.f, 900.f, 32.f, 32.f);
    m_meshRenderer->SetMesh(mesh);

    // ���͸��� ����
    m_material = GET_SINGLE(Resources)->Get<Material>(L"Lava")->Clone();
    m_meshRenderer->SetMaterial(m_material);

    // �ؽ�ó ����
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

    // �ʱ� ���� ����
    m_material->SetVec4(0, m_baseColor);
    m_material->SetVec4(1, m_highlightColor);
}

void Lava::Update()
{
    // �ð� ������Ʈ
    m_elapsedTime += DELTA_TIME;

    // ���̴� �Ķ���� ������Ʈ
    m_material->SetFloat(0, m_elapsedTime);     // ��� �ð�
    m_material->SetFloat(1, m_waveSpeed);       // �ĵ� �ӵ�
    m_material->SetFloat(2, m_waveStrength);    // �ĵ� ����
}