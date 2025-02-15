#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"
#include "PhysicsBody.h"

#include "PlayerCameraScript.h"
#include "Resources.h"
#include "ParticleSystem.h"
#include "Terrain.h"
#include "SphereCollider.h"
#include "MeshData.h"
#include "TestAnimation.h"
#include "PlayerManager.h"
#include "ProjectileManager.h"
#include "TeleportSystem.h"
#include "SoundSystem.h"
#include "TransformAnimator.h"
#include "Cactus.h"
#include "ChestMonster.h"
#include "BunnyRat.h"
#include "Ghost.h"
#include "Boss.h"
#include "Stone.h"
#include "KeyObject.h"
#include "Lava.h"
#include "Flame.h"


void SceneManager::Update()
{
	if (_activeScene == nullptr)
		return;

	_activeScene->Update();
	_activeScene->LateUpdate();
	_activeScene->FinalUpdate();
}

void SceneManager::Render()
{
	if (_activeScene)
		_activeScene->Render();
}

void SceneManager::LoadScene(wstring sceneName)
{
	_activeScene = LoadTestScene();
	if (_activeScene == nullptr) {
		Logger::Instance().Error("Scene 로드 실패");
	}
	GET_SINGLE(ProjectileManager)->Initialize();
	GET_SINGLE(TeleportSystem)->Initialize();

	// Main BGM 재생
	auto sound = GET_SINGLE(Resources)->Get<Sound>(L"MainStageBGM");
	if (sound) {
		GET_SINGLE(SoundSystem)->Play(sound);
	}

	_activeScene->Awake();
	_activeScene->Start();
}

void SceneManager::SetLayerName(uint8 index, const wstring& name)
{
	// 기존 데이터 삭제
	const wstring& prevName = _layerNames[index];
	_layerIndex.erase(prevName);

	_layerNames[index] = name;
	_layerIndex[name] = index;
}

uint8 SceneManager::LayerNameToIndex(const wstring& name)
{
	auto findIt = _layerIndex.find(name);
	if (findIt == _layerIndex.end())
		return 0;

	return findIt->second;
}

shared_ptr<GameObject> SceneManager::Pick(int32 screenX, int32 screenY)
{
	shared_ptr<Camera> camera = GetActiveScene()->GetMainCamera();

	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	Matrix projectionMatrix = camera->GetProjectionMatrix();

	// ViewSpace에서 Picking 진행
	float viewX = (+2.0f * screenX / width - 1.0f) / projectionMatrix(0, 0);
	float viewY = (-2.0f * screenY / height + 1.0f) / projectionMatrix(1, 1);

	Matrix viewMatrix = camera->GetViewMatrix();
	Matrix viewMatrixInv = viewMatrix.Invert();

	auto& gameObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjects();

	float minDistance = FLT_MAX;
	shared_ptr<GameObject> picked;

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetCollider() == nullptr)
			continue;

		// ViewSpace에서의 Ray 정의
		Vec4 rayOrigin = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		Vec4 rayDir = Vec4(viewX, viewY, 1.0f, 0.0f);

		// WorldSpace에서의 Ray 정의
		rayOrigin = XMVector3TransformCoord(rayOrigin, viewMatrixInv);
		rayDir = XMVector3TransformNormal(rayDir, viewMatrixInv);
		rayDir.Normalize();

		// WorldSpace에서 연산
		float distance = 0.f;
		if (gameObject->GetCollider()->Intersects(rayOrigin, rayDir, OUT distance) == false)
			continue;

		if (distance < minDistance)
		{
			minDistance = distance;
			picked = gameObject;
		}
	}

	return picked;
}

shared_ptr<Scene> SceneManager::LoadTestScene()
{
#pragma region LayerMask
	SetLayerName(0, L"Default");
	SetLayerName(1, L"UI");
#pragma endregion

#pragma region ComputeShader
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");

		// UAV 용 Texture 생성
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->CreateTexture(L"UAVTexture",
			DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"ComputeShader");
		material->SetShader(shader);
		material->SetInt(0, 1);
		GEngine->GetComputeDescHeap()->SetUAV(texture->GetUAVHandle(), UAV_REGISTER::u0);

		// 쓰레드 그룹 (1 * 1024 * 1)
		material->Dispatch(1, 1024, 1);
	}
#pragma endregion

	shared_ptr<Scene> scene = make_shared<Scene>();
	scene->ReserveGameObjects(200); // 이거 안해주면 망함
	
#pragma region Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Main_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, FOV=45도
		camera->AddComponent(make_shared<PlayerCameraScript>());
		camera->GetCamera()->SetFar(5000.f);
		camera->GetCamera()->SetMainCamera(true);
		camera->GetTransform()->SetLocalPosition(Vec3(-358.388f, 633.076f, 1065.6f));
		camera->GetTransform()->SetLocalRotation(Vec3(XMConvertToRadians(20.f), XMConvertToRadians(-180.f), XMConvertToRadians(0.f)));
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI는 안 찍음
		scene->AddGameObject(camera);
	}	
#pragma endregion

#pragma region UI_Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Orthographic_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, 800*600
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskAll(); // 다 끄고
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI만 찍음
		scene->AddGameObject(camera);
	}
#pragma endregion

#pragma region SkyBox
	{
		shared_ptr<GameObject> skybox = make_shared<GameObject>();
		skybox->AddComponent(make_shared<Transform>());
		skybox->SetCheckFrustum(false);
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Skybox");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Sky01", L"..\\Resources\\Texture\\Sky01.jpg");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		skybox->AddComponent(meshRenderer);
		scene->AddGameObject(skybox);
	}
#pragma endregion

#pragma region UI_Test
	for (int32 i = 0; i < 6; i++)
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->SetCheckFrustum(false);
		obj->AddComponent(make_shared<Transform>());
		obj->GetTransform()->SetLocalScale(Vec3(150.f, 100.f, 100.f));
		obj->GetTransform()->SetLocalPosition(Vec3(-600.f + (i * 155), 310.f, 500.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");

			shared_ptr<Texture> texture;
			if (i < 3)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->GetRTTexture(i);
			else if (i < 5)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->GetRTTexture(i - 3);
			else
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->GetRTTexture(0);

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		obj->AddComponent(meshRenderer);
		//scene->AddGameObject(obj);
	}
#pragma endregion

#pragma region Directional Light
	{
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(-4.49838f, 4000.f, -1171.68f));
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightDirection(Vec3(0.f, -1.f, 0.f));
		light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
		light->GetLight()->SetDiffuse(Vec3(1.0f, 1.0f, 1.0f));
		light->GetLight()->SetAmbient(Vec3(0.1f, 0.1f, 0.1f));
		light->GetLight()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));

		scene->AddGameObject(light);
	}
#pragma endregion

#pragma region FBX
	{
		shared_ptr<MeshData> mapMeshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Map2.fbx");
		shared_ptr<GameObject> mapObject = mapMeshData->Instantiate()[0];

		mapObject->SetName(L"Map");
		mapObject->SetCheckFrustum(false);
		mapObject->SetStatic(false);

		Vec3 Pos = Vec3(0.f, 0.f, 0.f);
		Vec3 Scale = Vec3(1.f, 1.f, 1.f);
		Vec3 Rotation = Vec3(-XM_PIDIV2, XM_PIDIV2, 0.f);

		mapObject->GetTransform()->SetLocalPosition(Pos);
		mapObject->GetTransform()->SetLocalScale(Scale);
		mapObject->GetTransform()->SetLocalRotation(Rotation);

		// PhysicsBody 컴포넌트 추가
		auto physicsBody = make_shared<PhysicsBody>();
		mapObject->AddComponent(physicsBody);
		
		// 메시 데이터 준비
		auto meshRenderer = mapObject->GetMeshRenderer();
		if (meshRenderer && meshRenderer->GetMesh()) {
			auto fbxMeshInfo = meshRenderer->GetMesh()->GetFbxMeshInfo();

			PhysicsBody::MeshParams params;
			for (const auto& vertex : fbxMeshInfo.vertices) {
				params.vertices.push_back(PxVec3(vertex.pos.x, vertex.pos.y, vertex.pos.z));
			}

			for (const auto& indices : fbxMeshInfo.indices) {
				params.indices.insert(params.indices.end(), indices.begin(), indices.end());
			}

			// 충돌 그룹 설정
			physicsBody->SetCollisionGroup(CollisionGroup::Ground);
			physicsBody->SetCollisionMask(
				CollisionGroup::Default |
				CollisionGroup::Player |
				CollisionGroup::Enemy |
				CollisionGroup::Projectile |
				CollisionGroup::Obstacle
			);

			// TriangleMesh 생성
			physicsBody->CreateBody(
				PhysicsObjectType::STATIC,
				PhysicsShapeType::TriangleMesh,
				params);

			physicsBody->GetPhysicsObject()->GetActor()->setName("MapObject");
			physicsBody->GetPhysicsObject()->GetActor()->userData = mapObject.get();
		}
		
		scene->AddGameObject(mapObject);

		// 플레이어 생성
		shared_ptr<MeshData> playerMeshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Orange.fbx");
		shared_ptr<GameObject> playerObj = playerMeshData->Instantiate()[0];
		int playerID = GEngine->GetMyPlayerId();
		Player* player = GET_SINGLE(PlayerManager)->CreatePlayer(playerID, playerObj);
		GET_SINGLE(SoundSystem)->SetPlayer(player);
		scene->AddGameObject(playerObj);

		// 총 부착
		{
			shared_ptr<MeshData> defaultGunMeshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\DefaultGun.fbx");
			shared_ptr<GameObject> defaultGunObject = defaultGunMeshData->Instantiate()[0];
			shared_ptr<GameObject> player = playerObj;
			defaultGunObject->SetName(L"defaultGun");
			defaultGunObject->SetCheckFrustum(false);
			defaultGunObject->SetStatic(false);
			defaultGunObject->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 40.f));
			defaultGunObject->GetTransform()->SetLocalRotation(Vec3(XMConvertToRadians(-45.f), XMConvertToRadians(-90.f), XMConvertToRadians(-30.f)));
			defaultGunObject->GetTransform()->SetLocalScale(Vec3(65.0f, 65.0f, 65.0f));
			defaultGunObject->AttachToBone(player, L"mixamorig:RightHand");
			scene->AddGameObject(defaultGunObject);
		}

		// 선인장 1
		{
			shared_ptr<MeshData> cactusMeshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\CactusPA.fbx");

			shared_ptr<GameObject> cactusMonsterObj = cactusMeshData->Instantiate()[0];
			cactusMonsterObj->SetName(L"Cactus1");
			cactusMonsterObj->SetCheckFrustum(true);
			cactusMonsterObj->SetStatic(false);
			cactusMonsterObj->GetTransform()->SetLocalPosition(Vec3(2218.504f, 233.49023f, -1718.1282f));
			cactusMonsterObj->GetTransform()->SetLocalRotation(Vec3(0.f, XM_PI, 0.f));
			cactusMonsterObj->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

			cactusMonsterObj->AddComponent(make_shared<Cactus>());
			scene->AddGameObject(cactusMonsterObj);
		}

		// 선인장 2
		{
			shared_ptr<MeshData> cactusMeshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\CactusPA.fbx");

			shared_ptr<GameObject> cactusMonsterObj = cactusMeshData->Instantiate()[0];
			cactusMonsterObj->SetName(L"Cactus2");
			cactusMonsterObj->SetCheckFrustum(true);
			cactusMonsterObj->SetStatic(false);
			cactusMonsterObj->GetTransform()->SetLocalPosition(Vec3(1943.4121f, 233.42471f, -1457.759f));
			cactusMonsterObj->GetTransform()->SetLocalRotation(Vec3(0.f, XM_PI, 0.f));
			cactusMonsterObj->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

			cactusMonsterObj->AddComponent(make_shared<Cactus>());
			scene->AddGameObject(cactusMonsterObj);
		}

		// 선인장 3
		{
			shared_ptr<MeshData> cactusMeshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\CactusPA.fbx");

			shared_ptr<GameObject> cactusMonsterObj = cactusMeshData->Instantiate()[0];
			cactusMonsterObj->SetName(L"Cactus3");
			cactusMonsterObj->SetCheckFrustum(true);
			cactusMonsterObj->SetStatic(false);
			cactusMonsterObj->GetTransform()->SetLocalPosition(Vec3(1750.2568f, 233.58658f, -1916.5537f));
			cactusMonsterObj->GetTransform()->SetLocalRotation(Vec3(0.f, XM_PI, 0.f));
			cactusMonsterObj->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

			cactusMonsterObj->AddComponent(make_shared<Cactus>());
			scene->AddGameObject(cactusMonsterObj);
		}

		// 미믹1
		{
			shared_ptr<MeshData> chestMonsterMeshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\ChestMonsterPBRDefault.fbx");

			shared_ptr<GameObject> chestMonsterObj = chestMonsterMeshData->Instantiate()[0];
			chestMonsterObj->SetName(L"ChestMonster1");
			chestMonsterObj->SetCheckFrustum(true);
			chestMonsterObj->SetStatic(false);
			chestMonsterObj->GetTransform()->SetLocalPosition(Vec3(2358.19f, 310.00018f, -3162.0906f));
			chestMonsterObj->GetTransform()->SetLocalRotation(Vec3(0.f, XM_PI, 0.f));
			chestMonsterObj->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

			chestMonsterObj->AddComponent(make_shared<ChestMonster>());
			scene->AddGameObject(chestMonsterObj);
		}

		// 미믹2
		{
			shared_ptr<MeshData> cactusMeshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\ChestMonsterPBRDefault.fbx");

			shared_ptr<GameObject> cactusMonsterObj = cactusMeshData->Instantiate()[0];
			cactusMonsterObj->SetName(L"ChestMonster2");
			cactusMonsterObj->SetCheckFrustum(true);
			cactusMonsterObj->SetStatic(false);
			cactusMonsterObj->GetTransform()->SetLocalPosition(Vec3(1564.7744f, 310.00012f, -3147.0874f));
			cactusMonsterObj->GetTransform()->SetLocalRotation(Vec3(0.f, XM_PI, 0.f));
			cactusMonsterObj->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

			cactusMonsterObj->AddComponent(make_shared<ChestMonster>());
			scene->AddGameObject(cactusMonsterObj);
		}

		// 미믹3
		{
			shared_ptr<MeshData> cactusMeshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\ChestMonsterPBRDefault.fbx");

			shared_ptr<GameObject> cactusMonsterObj = cactusMeshData->Instantiate()[0];
			cactusMonsterObj->SetName(L"ChestMonster3");
			cactusMonsterObj->SetCheckFrustum(true);
			cactusMonsterObj->SetStatic(false);
			cactusMonsterObj->GetTransform()->SetLocalPosition(Vec3(1552.766f, 310.000114f, -3737.4443f));
			cactusMonsterObj->GetTransform()->SetLocalRotation(Vec3(0.f, XM_PI, 0.f));
			cactusMonsterObj->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

			cactusMonsterObj->AddComponent(make_shared<ChestMonster>());
			scene->AddGameObject(cactusMonsterObj);
		}

		// 미믹4
		{
			shared_ptr<MeshData> cactusMeshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\ChestMonsterPBRDefault.fbx");

			shared_ptr<GameObject> cactusMonsterObj = cactusMeshData->Instantiate()[0];
			cactusMonsterObj->SetName(L"ChestMonster4");
			cactusMonsterObj->SetCheckFrustum(true);
			cactusMonsterObj->SetStatic(false);
			cactusMonsterObj->GetTransform()->SetLocalPosition(Vec3(2344.235f, 310.000175f, -3749.5261f));
			cactusMonsterObj->GetTransform()->SetLocalRotation(Vec3(0.f, XM_PI, 0.f));
			cactusMonsterObj->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

			cactusMonsterObj->AddComponent(make_shared<ChestMonster>());
			scene->AddGameObject(cactusMonsterObj);
		}

		// 토끼쥐 1
		{
			shared_ptr<MeshData> bunnyRatMeshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Bunny Rat Monster-Blue.fbx");

			shared_ptr<GameObject> bunnyRatObj = bunnyRatMeshData->Instantiate()[0];
			auto transformAnimator = bunnyRatObj->GetTransformAnimator();
			if (transformAnimator) {
				bunnyRatObj->RemoveComponent(transformAnimator);
			}

			bunnyRatObj->SetName(L"BunnyRat1");
			bunnyRatObj->SetCheckFrustum(true);
			bunnyRatObj->SetStatic(false);
			bunnyRatObj->GetTransform()->SetLocalPosition(Vec3(2152.0576f, 322.97729f, 283.64536f));
			bunnyRatObj->GetTransform()->SetLocalRotation(Vec3(0.f, XM_PI, 0.f));
			bunnyRatObj->GetTransform()->SetLocalScale(Vec3(1.5f, 1.5f, 1.5f));

			bunnyRatObj->AddComponent(make_shared<BunnyRat>());
			scene->AddGameObject(bunnyRatObj);
		}

		// 토끼쥐 2
		{
			shared_ptr<MeshData> bunnyRatMeshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Bunny Rat Monster-Blue.fbx");

			shared_ptr<GameObject> bunnyRatObj = bunnyRatMeshData->Instantiate()[0];
			auto transformAnimator = bunnyRatObj->GetTransformAnimator();
			if (transformAnimator) {
				bunnyRatObj->RemoveComponent(transformAnimator);
			}

			bunnyRatObj->SetName(L"BunnyRat2");
			bunnyRatObj->SetCheckFrustum(true);
			bunnyRatObj->SetStatic(false);
			bunnyRatObj->GetTransform()->SetLocalPosition(Vec3(1732.5071f, 329.4406f, 263.0665f));
			bunnyRatObj->GetTransform()->SetLocalRotation(Vec3(0.f, XM_PI, 0.f));
			bunnyRatObj->GetTransform()->SetLocalScale(Vec3(1.5f, 1.5f, 1.5f));

			bunnyRatObj->AddComponent(make_shared<BunnyRat>());
			scene->AddGameObject(bunnyRatObj);
		}

		// 토끼쥐 3
		{
			shared_ptr<MeshData> bunnyRatMeshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Bunny Rat Monster-Blue.fbx");

			shared_ptr<GameObject> bunnyRatObj = bunnyRatMeshData->Instantiate()[0];
			auto transformAnimator = bunnyRatObj->GetTransformAnimator();
			if (transformAnimator) {
				bunnyRatObj->RemoveComponent(transformAnimator);
			}

			bunnyRatObj->SetName(L"BunnyRat3");
			bunnyRatObj->SetCheckFrustum(true);
			bunnyRatObj->SetStatic(false);
			bunnyRatObj->GetTransform()->SetLocalPosition(Vec3(1584.6089f, 330.32803f, -64.49085f));
			bunnyRatObj->GetTransform()->SetLocalRotation(Vec3(0.f, XM_PI, 0.f));
			bunnyRatObj->GetTransform()->SetLocalScale(Vec3(1.5f, 1.5f, 1.5f));

			bunnyRatObj->AddComponent(make_shared<BunnyRat>());
			scene->AddGameObject(bunnyRatObj);
		}

		// 토끼쥐 4
		{
			shared_ptr<MeshData> bunnyRatMeshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Bunny Rat Monster-Blue.fbx");

			shared_ptr<GameObject> bunnyRatObj = bunnyRatMeshData->Instantiate()[0];
			auto transformAnimator = bunnyRatObj->GetTransformAnimator();
			if (transformAnimator) {
				bunnyRatObj->RemoveComponent(transformAnimator);
			}

			bunnyRatObj->SetName(L"BunnyRat4");
			bunnyRatObj->SetCheckFrustum(true);
			bunnyRatObj->SetStatic(false);
			bunnyRatObj->GetTransform()->SetLocalPosition(Vec3(2341.3164f, 322.479034f, -81.876396f));
			bunnyRatObj->GetTransform()->SetLocalRotation(Vec3(0.f, XM_PI, 0.f));
			bunnyRatObj->GetTransform()->SetLocalScale(Vec3(1.5f, 1.5f, 1.5f));

			bunnyRatObj->AddComponent(make_shared<BunnyRat>());
			scene->AddGameObject(bunnyRatObj);
		}

		// 유령1
		{
			shared_ptr<MeshData> ghostMeshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Ghost_animation.fbx");

			shared_ptr<GameObject> ghostObj = ghostMeshData->Instantiate()[0];

			ghostObj->SetName(L"Ghost1");
			ghostObj->SetCheckFrustum(true);
			ghostObj->SetStatic(false);
			ghostObj->GetTransform()->SetLocalPosition(Vec3(-1652.3177f, 359.999866f, 1738.5172f));
			ghostObj->GetTransform()->SetLocalRotation(Vec3(-XM_PIDIV2, XM_PI, 0.f));
			ghostObj->GetTransform()->SetLocalScale(Vec3(1.5f, 1.5f, 1.5f));

			ghostObj->AddComponent(make_shared<Ghost>());
			scene->AddGameObject(ghostObj);
		}

		// 유령2
		{
			shared_ptr<MeshData> ghostMeshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Ghost_animation.fbx");
			shared_ptr<GameObject> ghostObj = ghostMeshData->Instantiate()[0];

			ghostObj->SetName(L"Ghost2");
			ghostObj->SetCheckFrustum(true);
			ghostObj->SetStatic(false);
			ghostObj->GetTransform()->SetLocalPosition(Vec3(-2256.46f, 359.99983f, 1741.7324f));
			ghostObj->GetTransform()->SetLocalRotation(Vec3(-XM_PIDIV2, XM_PI, 0.f));
			ghostObj->GetTransform()->SetLocalScale(Vec3(1.5f, 1.5f, 1.5f));

			ghostObj->AddComponent(make_shared<Ghost>());
			scene->AddGameObject(ghostObj);
		}

		// 보스
		{
			shared_ptr<MeshData> bossMeshData = 
				GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\DemonBoss.fbx");

			shared_ptr<GameObject> bossObj = bossMeshData->Instantiate()[0];

			bossObj->SetName(L"Boss");
			bossObj->SetCheckFrustum(true);
			bossObj->SetStatic(false);
			bossObj->GetTransform()->SetLocalPosition(Vec3(-5.0f, 335.0f, -4000.0f));
			bossObj->GetTransform()->SetLocalRotation(Vec3(0.f, XM_PI, 0.f));
			bossObj->GetTransform()->SetLocalScale(Vec3(1.5f, 1.5f, 1.5f));

			bossObj->AddComponent(make_shared<Boss>());
			scene->AddGameObject(bossObj);
		}

		// Stone
		{
			shared_ptr<MeshData> stoneMeshData =
				GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\spike_cube_no_animated.fbx");

			shared_ptr<GameObject> bossObj = stoneMeshData->Instantiate()[0];

			bossObj->SetName(L"Stone");
			bossObj->SetCheckFrustum(true);
			bossObj->SetStatic(false);
			bossObj->GetTransform()->SetLocalPosition(Vec3(-1600.0f, 260.0f, -1500.0f));
			bossObj->GetTransform()->SetLocalRotation(Vec3(0.f, 0.f, 0.f));
			bossObj->GetTransform()->SetLocalScale(Vec3(1.0f, 1.0f, 1.0f));

			bossObj->AddComponent(make_shared<Stone>());
			scene->AddGameObject(bossObj);
		}

		{
			for (const auto& position : KeyObject::KEY_POSITIONS) {
				shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\key_gold.fbx");
				shared_ptr<GameObject> gameObject = meshData->Instantiate()[0];

				{
					gameObject->SetCheckFrustum(true);
					gameObject->SetStatic(false);
					gameObject->GetTransform()->SetLocalPosition(Vec3(position));
					gameObject->GetTransform()->SetLocalRotation(Vec3(XM_PIDIV2, XM_PIDIV2, 0.f));
					gameObject->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
					gameObject->AddComponent(make_shared<KeyObject>());
					scene->AddGameObject(gameObject);
				}
			}
		}

		{
			auto lavaObject = make_shared<GameObject>();
			lavaObject->SetName(L"Lava");
			lavaObject->AddComponent(make_shared<Transform>());
			lavaObject->AddComponent(make_shared<Lava>());

			// 위치와 크기 설정
			auto transform = lavaObject->GetTransform();
			transform->SetLocalPosition(Vec3(-1903.796f, -100.f, -50.02089f));
			transform->SetLocalScale(Vec3(1.0f, 1.0f, 1.0f));
			transform->SetLocalRotation(Vec3(XM_PIDIV2, 0.0f, 0.0f));

			auto sound = GET_SINGLE(Resources)->Get<Sound>(L"Lava");
			if (sound) {
				GET_SINGLE(SoundSystem)->Play3D(sound, Vec3(-1903.796f, -100.f, -50.02089f));
			}

			scene->AddGameObject(lavaObject);
		}

		{
			auto flameObject = make_shared<GameObject>();
			flameObject->SetName(L"Flame1");
			flameObject->AddComponent(make_shared<Transform>());
			flameObject->AddComponent(make_shared<Flame>());

			// 위치와 크기 설정
			auto transform = flameObject->GetTransform();
			transform->SetLocalPosition(Vec3(290.f, 250.f, -4000.f));
			transform->SetLocalScale(Vec3(1.0f, 1.0f, 1.0f));
			transform->SetLocalRotation(Vec3(0.f, 0.0f, 0.0f));

			scene->AddGameObject(flameObject);
		}

		{
			auto flameObject = make_shared<GameObject>();
			flameObject->SetName(L"Flame2");
			flameObject->AddComponent(make_shared<Transform>());
			flameObject->AddComponent(make_shared<Flame>());

			// 위치와 크기 설정
			auto transform = flameObject->GetTransform();
			transform->SetLocalPosition(Vec3(-310.f, 250.f, -4000.f));
			transform->SetLocalScale(Vec3(1.0f, 1.0f, 1.0f));
			transform->SetLocalRotation(Vec3(0.f, 0.0f, 0.0f));

			scene->AddGameObject(flameObject);
		}


#pragma region FadeOutObject
		vector<Vec3> stagePositions = {
			Vec3(0.f,		  0.f, 0.f),
			Vec3(7.865564f,   0.f, -1745.3679f),
			Vec3(1944.9496f,  0.f, -1696.3253f),
			Vec3(1955.0245f,  0.f, -6.5450735f),
			Vec3(1944.4672f,  0.f, -3457.5078f),
			Vec3(-1956.9929f, 0.f, -1798.4175f),
			Vec3(-2049.9458f, 0.f, -106.035675f),
			Vec3(-2003.9424f, 0.f, 1490.736f) 
		};

		for (const auto& position : stagePositions) {
			CreateFadeOutObjects(
				position,
				1500.f, 500.f, 570.f,
				750.f, 750.f, 1.5f,
				scene);
		}

		CreateFadeOutObjects(
			Vec3(-8.31838f, 110.f, -3500.4863f),
			1500.f, 500.f, 650.f,
			1000.f, 750.f, 1.5f,
			scene);
#pragma endregion

	}
#pragma endregion

	return scene;
}

void SceneManager::CreateFadeOutObjects(
	const Vec3& position, 
	float width, float height, float y, 
	float vOffset, float hOffset, float yOffset,
	shared_ptr<Scene>& scene)
{
	// 각 객체의 위치와 회전 값을 저장
	vector<Vec3> positions = {
		Vec3(position.x,			y + yOffset * 0,	position.z + vOffset),
		Vec3(position.x + hOffset,	y + yOffset * 1,	position.z),
		Vec3(position.x - hOffset,	y + yOffset * 2,	position.z),
		Vec3(position.x,			y + yOffset * 3,	position.z -vOffset)
	};

	vector<float> rotationYs = {
		XMConvertToRadians(0.f),
		XMConvertToRadians(90.f),
		XMConvertToRadians(270.f),
		XMConvertToRadians(180.f)
	};

	// 반복문을 사용하여 객체 생성
	for (size_t i = 0; i < positions.size(); ++i)
	{
		shared_ptr<GameObject> fadeOutObject = make_shared<GameObject>();
		fadeOutObject->AddComponent(make_shared<Transform>());
		fadeOutObject->AddComponent(make_shared<MeshRenderer>());
		fadeOutObject->SetCheckFrustum(false);

		auto meshRenderer = fadeOutObject->GetMeshRenderer();
		meshRenderer->SetMesh(GET_SINGLE(Resources)->LoadRectangleMesh());

		auto material = GET_SINGLE(Resources)->Get<Material>(L"FadeOut")->Clone();
		material->SetVec2(0, Vec2(0.f, 1.f));
		meshRenderer->SetMaterial(material);

		fadeOutObject->GetTransform()->SetLocalPosition(positions[i]);
		fadeOutObject->GetTransform()->SetLocalScale(
			Vec3(width,
				height,
				1.f));
		fadeOutObject->GetTransform()->SetLocalRotation(
			Vec3(XMConvertToRadians(90.f),
				rotationYs[i],
				0.f));

		scene->AddGameObject(fadeOutObject);
	}
}
