#include "pch.h"
#include "Flame.h"
#include "GameObject.h"
#include "Resources.h"
#include "MeshRenderer.h"
#include "Timer.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Transform.h"

Flame::Flame()
{
}

Flame::~Flame()
{
}

void Flame::Awake()
{
	CreateComponents();
}

void Flame::Update()
{
	m_elapsedTime += DELTA_TIME;
	UpdateFlameEffect();
}

void Flame::CreateComponents()
{
    Vec3 position = GetGameObject()->GetTransform()->GetLocalPosition();

    // �Ҳ� ������Ʈ ����
    m_flameObject = make_shared<GameObject>();
    m_flameObject->AddComponent(make_shared<Transform>());
    m_flameRenderer = make_shared<MeshRenderer>();
    m_flameObject->AddComponent(m_flameRenderer);
    m_flameObject->GetTransform()->SetLocalPosition(position);

    // �޽� ����
    auto flameMesh = GET_SINGLE(Resources)->LoadRectangleMesh(100.f, 200.f, 2, 2);
    m_flameRenderer->SetMesh(flameMesh);

    // �Ҳ� ���͸��� ����
    m_flameMaterial = GET_SINGLE(Resources)->Get<Material>(L"Flame")->Clone();
    m_flameRenderer->SetMaterial(m_flameMaterial);

    // ���͸��� �ؽ�ó ����
    m_flameMaterial->SetTexture(0, GET_SINGLE(Resources)->Load<Texture>(
        L"FlameBase", L"..\\Resources\\Texture\\flame_base.png"));
    m_flameMaterial->SetTexture(1, GET_SINGLE(Resources)->Load<Texture>(
        L"NoiseTexture2", L"..\\Resources\\Texture\\noise1.jpg"));
    m_flameMaterial->SetTexture(2, GET_SINGLE(Resources)->Load<Texture>(
        L"FlameGradient", L"..\\Resources\\Texture\\flame_gradient.png"));

    // �ʱ� �Ķ���� ����
    m_flameMaterial->SetVec4(0, m_params.baseColor);
    m_flameMaterial->SetVec4(1, m_params.tipColor);
    m_flameMaterial->SetVec4(2, Vec4(m_params.flickerSpeed, m_params.flickerIntensity, m_params.verticalSpeed, m_params.noiseScale));

	GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(m_flameObject);


    // ���� ������Ʈ ����
	m_smokeObject = make_shared<GameObject>();
	m_smokeObject->AddComponent(make_shared<Transform>());
	m_smokeRenderer = make_shared<MeshRenderer>();
	m_smokeObject->AddComponent(m_smokeRenderer);
    m_smokeObject->GetTransform()->SetLocalPosition(position);

    // �޽� ����
    auto smokeMesh = GET_SINGLE(Resources)->LoadRectangleMesh(200.f, 400.f, 2, 2);
	m_smokeRenderer->SetMesh(smokeMesh);

    // ���� ���͸��� ����
	m_smokeMaterial = GET_SINGLE(Resources)->Get<Material>(L"Smoke")->Clone();
	m_smokeRenderer->SetMaterial(m_smokeMaterial);

	// ���͸��� �ؽ�ó ����
	m_smokeMaterial->SetTexture(0, GET_SINGLE(Resources)->Load<Texture>(
		L"SmokeTexture", L"..\\Resources\\Texture\\smoke.png"));
    m_smokeMaterial->SetTexture(1, GET_SINGLE(Resources)->Load<Texture>(
        L"NoiseTexture", L"..\\Resources\\Texture\\noise2.png"));

    // �ʱ� �Ķ���� ����
	m_smokeMaterial->SetVec4(0, m_smokeParams.smokeColor);
	m_smokeMaterial->SetVec4(1, Vec4(m_smokeParams.riseSpeed, m_smokeParams.fadeSpeed, m_smokeParams.turbulence, 0.f));

	GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(m_smokeObject);
}

void Flame::UpdateFlameEffect()
{
    // �ð� ������Ʈ
    m_flameMaterial->SetFloat(0, m_elapsedTime);
    m_smokeMaterial->SetFloat(0, m_elapsedTime);

    // ī�޶� ���� ���
    auto cameraTransform = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetTransform();
    Vec3 cameraPosition = cameraTransform->GetLocalPosition();

    // �Ҳ� ������ ������Ʈ
    auto flameTransform = m_flameObject->GetTransform();
    Vec3 flamePosition = flameTransform->GetLocalPosition();
    Vec3 dirToCamera = cameraPosition - flamePosition;
    dirToCamera.y = 0.f;  // Y�� ȸ���� ����
    dirToCamera.Normalize();
    flameTransform->LookAt(dirToCamera);

    // ���� ������ ������Ʈ
    auto smokeTransform = m_smokeObject->GetTransform();
    Vec3 smokePosition = smokeTransform->GetLocalPosition();
    dirToCamera = cameraPosition - smokePosition;
    dirToCamera.y = 0.f;
    dirToCamera.Normalize();
    smokeTransform->LookAt(dirToCamera);

    // ���� ��ġ ���� (�Ҳ� ���� ��ġ)
    smokePosition = flamePosition + Vec3(0.f, 50.f, 0.f);  // �Ҳɺ��� �ణ ���� ��ġ
    smokeTransform->SetLocalPosition(smokePosition);
}
