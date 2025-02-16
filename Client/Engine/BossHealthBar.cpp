#include "pch.h"
#include "BossHealthBar.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Resources.h"
#include "SceneManager.h"
#include "Scene.h"

BossHealthBar::BossHealthBar()
{
}

BossHealthBar::~BossHealthBar()
{
}

void BossHealthBar::Start()
{
	auto scene = GET_SINGLE(SceneManager)->GetActiveScene();

	_fillBar = make_shared<GameObject>();
	_fillBar->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
	_fillBar->SetCheckFrustum(false);
	_fillBar->AddComponent(make_shared<Transform>());
	_fillBar->GetTransform()->SetLocalScale(Vec3(800.f, 30.f, 1.f));
	_fillBar->GetTransform()->SetLocalPosition(Vec3(0.f, -300.f, -100.f));

	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
	meshRenderer->SetMesh(mesh);

	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"boss_hp_bar", L"..\\Resources\\Texture\\boss_hp_bar.png");
	shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"UI")->Clone();
	material->SetTexture(0, texture);
	meshRenderer->SetMaterial(material);

	_fillBar->AddComponent(meshRenderer);
	scene->AddGameObject(_fillBar);
}

void BossHealthBar::Update()
{
	//// 체력바 위치와 크기 업데이트
	auto fillTransform = _fillBar->GetTransform();

	fillTransform->SetLocalScale(Vec3(800.f * _currentHealth / _maxHealth, 30.f, 1.f));
	fillTransform->SetLocalPosition(
		Vec3(-800.f * (1.0f - _currentHealth / _maxHealth) * 0.5f, 
			_fillBarPosition.y,
			_fillBarPosition.z));
}

void BossHealthBar::SetCurrentHealth(float currentHealth)
{
	_currentHealth = std::clamp(currentHealth, 0.01f, _maxHealth);
}
