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

    // 불꽃 오브젝트 설정
    m_flameObject = make_shared<GameObject>();
    m_flameObject->AddComponent(make_shared<Transform>());
    m_flameRenderer = make_shared<MeshRenderer>();
    m_flameObject->AddComponent(m_flameRenderer);
    m_flameObject->GetTransform()->SetLocalPosition(position);

    // 메시 설정
    auto flameMesh = GET_SINGLE(Resources)->LoadRectangleMesh(100.f, 200.f, 2, 2);
    m_flameRenderer->SetMesh(flameMesh);

    // 불꽃 머터리얼 설정
    m_flameMaterial = GET_SINGLE(Resources)->Get<Material>(L"Flame")->Clone();
    m_flameRenderer->SetMaterial(m_flameMaterial);

    // 머터리얼 텍스처 설정
    m_flameMaterial->SetTexture(0, GET_SINGLE(Resources)->Load<Texture>(
        L"FlameBase", L"..\\Resources\\Texture\\flame_base.png"));
    m_flameMaterial->SetTexture(1, GET_SINGLE(Resources)->Load<Texture>(
        L"NoiseTexture2", L"..\\Resources\\Texture\\noise1.jpg"));
    m_flameMaterial->SetTexture(2, GET_SINGLE(Resources)->Load<Texture>(
        L"FlameGradient", L"..\\Resources\\Texture\\flame_gradient.png"));

    // 초기 파라미터 설정
    m_flameMaterial->SetVec4(0, m_params.baseColor);
    m_flameMaterial->SetVec4(1, m_params.tipColor);
    m_flameMaterial->SetVec4(2, Vec4(m_params.flickerSpeed, m_params.flickerIntensity, m_params.verticalSpeed, m_params.noiseScale));

	GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(m_flameObject);


    // 연기 오브젝트 설정
	m_smokeObject = make_shared<GameObject>();
	m_smokeObject->AddComponent(make_shared<Transform>());
	m_smokeRenderer = make_shared<MeshRenderer>();
	m_smokeObject->AddComponent(m_smokeRenderer);
    m_smokeObject->GetTransform()->SetLocalPosition(position);

    // 메시 설정
    auto smokeMesh = GET_SINGLE(Resources)->LoadRectangleMesh(200.f, 400.f, 2, 2);
	m_smokeRenderer->SetMesh(smokeMesh);

    // 연기 머터리얼 설정
	m_smokeMaterial = GET_SINGLE(Resources)->Get<Material>(L"Smoke")->Clone();
	m_smokeRenderer->SetMaterial(m_smokeMaterial);

	// 머터리얼 텍스처 설정
	m_smokeMaterial->SetTexture(0, GET_SINGLE(Resources)->Load<Texture>(
		L"SmokeTexture", L"..\\Resources\\Texture\\smoke.png"));
    m_smokeMaterial->SetTexture(1, GET_SINGLE(Resources)->Load<Texture>(
        L"NoiseTexture", L"..\\Resources\\Texture\\noise2.png"));

    // 초기 파라미터 설정
	m_smokeMaterial->SetVec4(0, m_smokeParams.smokeColor);
	m_smokeMaterial->SetVec4(1, Vec4(m_smokeParams.riseSpeed, m_smokeParams.fadeSpeed, m_smokeParams.turbulence, 0.f));

	GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(m_smokeObject);
}

void Flame::UpdateFlameEffect()
{
    // 시간 업데이트
    m_flameMaterial->SetFloat(0, m_elapsedTime);
    m_smokeMaterial->SetFloat(0, m_elapsedTime);

    // 카메라 방향 계산
    auto cameraTransform = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetTransform();
    Vec3 cameraPosition = cameraTransform->GetLocalPosition();

    // 불꽃 빌보드 업데이트
    auto flameTransform = m_flameObject->GetTransform();
    Vec3 flamePosition = flameTransform->GetLocalPosition();
    Vec3 dirToCamera = cameraPosition - flamePosition;
    dirToCamera.y = 0.f;  // Y축 회전만 적용
    dirToCamera.Normalize();
    flameTransform->LookAt(dirToCamera);

    // 연기 빌보드 업데이트
    auto smokeTransform = m_smokeObject->GetTransform();
    Vec3 smokePosition = smokeTransform->GetLocalPosition();
    dirToCamera = cameraPosition - smokePosition;
    dirToCamera.y = 0.f;
    dirToCamera.Normalize();
    smokeTransform->LookAt(dirToCamera);

    // 연기 위치 조정 (불꽃 위에 배치)
    smokePosition = flamePosition + Vec3(0.f, 50.f, 0.f);  // 불꽃보다 약간 위에 배치
    smokeTransform->SetLocalPosition(smokePosition);
}
