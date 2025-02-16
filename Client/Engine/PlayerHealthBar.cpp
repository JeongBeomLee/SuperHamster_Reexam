#include "pch.h"
#include "PlayerHealthBar.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Resources.h"
#include "SceneManager.h"
#include "Scene.h"

PlayerHealthBar::PlayerHealthBar()
{
}

PlayerHealthBar::~PlayerHealthBar()
{
}

void PlayerHealthBar::Start()
{
	auto scene = GET_SINGLE(SceneManager)->GetActiveScene();

	// HP 바
	{
		_fillBar = make_shared<GameObject>();
		_fillBar->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		_fillBar->SetCheckFrustum(false);
		_fillBar->AddComponent(make_shared<Transform>());
		_fillBar->GetTransform()->SetLocalScale(Vec3(285.f, 25.f, 1.f));
		_fillBar->GetTransform()->SetLocalPosition(Vec3(-465.f, 315.f, 500.f));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		meshRenderer->SetMesh(mesh);

		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"player_hp_bar", L"..\\Resources\\Texture\\player_hp_bar.png");
		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"UI")->Clone();
		material->SetTexture(0, texture);
		meshRenderer->SetMaterial(material);

		_fillBar->AddComponent(meshRenderer);
		scene->AddGameObject(_fillBar);
	}

	// 아웃라인
	{
		_outLineBar = make_shared<GameObject>();
		_outLineBar->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
		_outLineBar->SetCheckFrustum(false);
		_outLineBar->AddComponent(make_shared<Transform>());
		_outLineBar->GetTransform()->SetLocalScale(Vec3(300.f, 30.f, 1.f));
		_outLineBar->GetTransform()->SetLocalPosition(Vec3(-465.f, 315.f, 500.f));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		meshRenderer->SetMesh(mesh);

		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"player_hp_outline", L"..\\Resources\\Texture\\player_hp_outline.png");
		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"UI")->Clone();
		material->SetTexture(0, texture);
		meshRenderer->SetMaterial(material);

		_outLineBar->AddComponent(meshRenderer);
		scene->AddGameObject(_outLineBar);
	}
}

void PlayerHealthBar::Update()
{
	//// 체력바 위치와 크기 업데이트
	auto fillTransform = _fillBar->GetTransform();

	fillTransform->SetLocalScale(Vec3(285.f * _currentHealth / _maxHealth, 25.f, 1.f));
	fillTransform->SetLocalPosition(Vec3(-465.f - 285.f * (1.0f - _currentHealth / _maxHealth) * 0.5f, 315.f, 500.f));
}

void PlayerHealthBar::SetCurrentHealth(float currentHealth)
{
	_currentHealth = std::clamp(currentHealth, 0.01f, _maxHealth);
}