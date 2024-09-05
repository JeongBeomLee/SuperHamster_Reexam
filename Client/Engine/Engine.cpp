#include "pch.h"
#include "Engine.h"
#include "Material.h"
#include "Transform.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Light.h"
#include "Resources.h"
#include "InstancingManager.h"
#include "Scene.h"
#include "Camera.h"
#include "PlayerMove.h"
#include "PlayerManager.h"

Engine::Engine()
{
}

Engine::~Engine()
{
	_controllerManager->release();
	_scene->release();
	_cpuDispatcher->release();
	_physics->release();
	_foundation->release();
}

void Engine::Init(const WindowInfo& info)
{
	_window = info;	

	// 그려질 화면 크기를 설정
	_viewport = { 0, 0, static_cast<FLOAT>(info.width), static_cast<FLOAT>(info.height), 0.0f, 1.0f };
	_scissorRect = CD3DX12_RECT(0, 0, info.width, info.height);

	_device->Init();
	_graphicsCmdQueue->Init(_device->GetDevice(), _swapChain);
	_computeCmdQueue->Init(_device->GetDevice());
	_swapChain->Init(info, _device->GetDevice(), _device->GetDXGI(), _graphicsCmdQueue->GetCmdQueue());
	_rootSignature->Init();
	_graphicsDescHeap->Init(256 * 12);
	_computeDescHeap->Init();

	CreateConstantBuffer(CBV_REGISTER::b0, sizeof(LightParams), 1);
	CreateConstantBuffer(CBV_REGISTER::b1, sizeof(TransformParams), 256 * 12);
	CreateConstantBuffer(CBV_REGISTER::b2, sizeof(MaterialParams), 256 * 12);

	CreateRenderTargetGroups();

	GET_SINGLE(Input)->Init(info.hwnd);
	GET_SINGLE(Timer)->Init();
	GET_SINGLE(Resources)->Init();

	InitializePhysics();
}

void Engine::Update()
{
	GET_SINGLE(Input)->Update();
	GET_SINGLE(Timer)->Update();
	GET_SINGLE(PlayerManager)->Update();
	GET_SINGLE(SceneManager)->Update();
	GET_SINGLE(InstancingManager)->ClearBuffer();

	Render();
	ShowFps();

	//UpdatePhysics();
}

void Engine::ToggleFullscreen()
{
	_window.windowed = !_window.windowed;
	bool fullscreen = !_window.windowed;

	if (fullscreen)
	{
		_window.width = GetSystemMetrics(SM_CXSCREEN);
		_window.height = GetSystemMetrics(SM_CYSCREEN);

		// 윈도우를 전체 화면 모드로 전환
		::SetWindowLongPtr(_window.hwnd, GWL_STYLE, WS_POPUP);
		::SetWindowPos(_window.hwnd, HWND_TOP, 0, 0, _window.width, _window.height, SWP_FRAMECHANGED);
		::ShowWindow(_window.hwnd, SW_MAXIMIZE);

		// 전체 화면 모드로 전환
		DXGI_MODE_DESC fullscreenMode = {};
		fullscreenMode.Width = _window.width;
		fullscreenMode.Height = _window.height;
		fullscreenMode.RefreshRate = { 60, 1 };
		fullscreenMode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		_swapChain->GetSwapChain()->ResizeTarget(&fullscreenMode);
	}
	else
	{
		_window.width = 1280;
		_window.height = 720;

		// 윈도우를 창 모드로 전환
		::SetWindowLongPtr(_window.hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		RECT rect = { 0, 0, _window.width, _window.height };
		::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
		::SetWindowPos(_window.hwnd, 0, 100, 100, rect.right - rect.left, rect.bottom - rect.top, 0);
		::ShowWindow(_window.hwnd, SW_SHOWNORMAL);

		// 창 모드로 전환
		DXGI_MODE_DESC windowedMode = {};
		windowedMode.Width = _window.width;
		windowedMode.Height = _window.height;
		windowedMode.RefreshRate = { 60, 1 };
		windowedMode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		_swapChain->GetSwapChain()->ResizeTarget(&windowedMode);
	}

	// 그려질 화면 크기를 설정
	_viewport = { 0, 0, static_cast<FLOAT>(_window.width), static_cast<FLOAT>(_window.height), 0.0f, 1.0f };
	_scissorRect = CD3DX12_RECT(0, 0, _window.width, _window.height);
}

void Engine::LoadMapMeshForPhysics(const shared_ptr<MeshData>& meshData)
{
	vector<MeshRenderInfo> meshRenders = meshData->GetMeshRenders();
	physx::PxCookingParams params(_physics->getTolerancesScale());
	params.meshPreprocessParams = physx::PxMeshPreprocessingFlags(physx::PxMeshPreprocessingFlag::eWELD_VERTICES);
	params.midphaseDesc = physx::PxMeshMidPhase::eBVH34;
	params.convexMeshCookingType = physx::PxConvexMeshCookingType::eQUICKHULL;
	params.buildGPUData = true;
	params.meshWeldTolerance = 0.001f;
	
	for (MeshRenderInfo& info : meshRenders)
	{
		shared_ptr<Mesh> mesh = info.mesh;
		FbxMeshInfo fbxMeshInfo = mesh->GetFbxMeshInfo();

		std::vector<Vertex> vertices = fbxMeshInfo.vertices;
		std::vector<physx::PxVec3> pxVertices;
		for(const Vertex& vertex : vertices)
		{
			pxVertices.emplace_back(vertex.pos.x, vertex.pos.y, vertex.pos.z);
		}

		std::vector<std::vector<uint32>> indices = fbxMeshInfo.indices;
		std::vector<physx::PxU32> pxIndices;
		for (const std::vector<uint32>& index : indices)
		{
			for (uint32 i : index)
			{
				pxIndices.emplace_back(i);
			}
		}

		//SaveMeshDataToBinary(pxVertices, pxIndices, "MeshData.bin"); // 저장하고 싶으면 주석 풀기

		physx::PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = static_cast<physx::PxU32>(pxVertices.size());
		meshDesc.points.stride = sizeof(physx::PxVec3);
		meshDesc.points.data = pxVertices.data();
		meshDesc.triangles.count = static_cast<physx::PxU32>(pxIndices.size() / 3);
		meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
		meshDesc.triangles.data = pxIndices.data();

		// PxTriangleMeshDesc를 직렬화하기 위한 메모리 스트림 생성
		physx::PxDefaultMemoryOutputStream writeBuffer;
		physx::PxTriangleMeshCookingResult::Enum result;
		bool status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);
		if (!status) {
			std::cerr << "Failed to cook triangle mesh." << std::endl;
		}

		if (writeBuffer.getSize() == 0) {
			std::cerr << "WriteBuffer is empty." << std::endl;
		}

		// 직렬화된 데이터를 PxInputStream으로 변환
		physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		physx::PxTriangleMesh* triangleMesh = _physics->createTriangleMesh(readBuffer);

		physx::PxMeshScale meshScale(physx::PxVec3(1, 1, 1));
		physx::PxTriangleMeshGeometry triangleMeshGeometry(triangleMesh, meshScale);
		physx::PxRigidStatic* triangleMeshActor = _physics->createRigidStatic(physx::PxTransform(physx::PxVec3(0, 0, 0)));
		physx::PxShape* triangleMeshShape = _physics->createShape(triangleMeshGeometry, *_physics->createMaterial(0.5f, 0.5f, 0.1f));

		// x축 기준 -90도 회전
		physx::PxQuat rotX = physx::PxQuat(-XM_PIDIV2, physx::PxVec3(1, 0, 0));
		physx::PxTransform transform(physx::PxVec3(0, 0, 0), rotX);
		if (!transform.isValid()) {
			std::cerr << "Invalid PxTransform detected." << std::endl;
			return;
		}
		triangleMeshActor->setGlobalPose(transform);

		triangleMeshActor->attachShape(*triangleMeshShape);
		_scene->addActor(*triangleMeshActor);

		// 메모리 해제
		triangleMeshShape->release();
		triangleMesh->release();
	}

	// 플레이어 컨트롤러 생성 (게임 시작하고 서버에서 위치를 받아야함.)
	/*physx::PxCapsuleControllerDesc desc;
	desc.height = 50.f;
	desc.radius = 25.f;
	desc.material = _physics->createMaterial(0.5f, 0.5f, 0.1f);
	desc.position = physx::PxExtendedVec3(-460.224, 300, 60.2587);
	_playerController = _controllerManager->createController(desc);*/

	std::cout << "Physics initialized" << std::endl;
}

void Engine::SaveMeshDataToBinary(const std::vector<physx::PxVec3>& vertices, const std::vector<physx::PxU32>& indices, const std::string& filePath)
{
	std::ofstream outFile(filePath, std::ios::binary);
	if (!outFile.is_open()) {
		std::cerr << "Failed to open file for writing: " << filePath << std::endl;
		return;
	}

	// Save vertex count and vertices
	physx::PxU32 vertexCount = static_cast<physx::PxU32>(vertices.size());
	outFile.write(reinterpret_cast<const char*>(&vertexCount), sizeof(physx::PxU32));
	outFile.write(reinterpret_cast<const char*>(vertices.data()), vertexCount * sizeof(physx::PxVec3));

	// Save index count and indices
	physx::PxU32 indexCount = static_cast<physx::PxU32>(indices.size());
	outFile.write(reinterpret_cast<const char*>(&indexCount), sizeof(physx::PxU32));
	outFile.write(reinterpret_cast<const char*>(indices.data()), indexCount * sizeof(physx::PxU32));

	outFile.close();
}

void Engine::Render()
{
	RenderBegin();

	GET_SINGLE(SceneManager)->Render();

	RenderEnd();
}

void Engine::RenderBegin()
{
	_graphicsCmdQueue->RenderBegin();
}

void Engine::RenderEnd()
{
	_graphicsCmdQueue->RenderEnd();
}

void Engine::ResizeWindow(int32 width, int32 height)
{
	_window.width = width;
	_window.height = height;

	RECT rect = { 0, 0, width, height };
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	::SetWindowPos(_window.hwnd, 0, 100, 100, width, height, 0);
}

void Engine::ShowFps()
{
	uint32 fps = GET_SINGLE(Timer)->GetFps();

	WCHAR text[100] = L"";
	::wsprintf(text, L"FPS : %d", fps);

	::SetWindowText(_window.hwnd, text);
}

void Engine::CreateConstantBuffer(CBV_REGISTER reg, uint32 bufferSize, uint32 count)
{
	uint8 typeInt = static_cast<uint8>(reg);
	assert(_constantBuffers.size() == typeInt);

	shared_ptr<ConstantBuffer> buffer = make_shared<ConstantBuffer>();
	buffer->Init(reg, bufferSize, count);
	_constantBuffers.push_back(buffer);
}


void Engine::CreateRenderTargetGroups()
{
	// DepthStencil
	shared_ptr<Texture> dsTexture = GET_SINGLE(Resources)->CreateTexture(L"DepthStencil",
		DXGI_FORMAT_D32_FLOAT, _window.width, _window.height,
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	// SwapChain Group
	{
		vector<RenderTarget> rtVec(SWAP_CHAIN_BUFFER_COUNT);

		for (uint32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
		{
			wstring name = L"SwapChainTarget_" + std::to_wstring(i);

			ComPtr<ID3D12Resource> resource;
			_swapChain->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&resource));
			rtVec[i].target = GET_SINGLE(Resources)->CreateTextureFromResource(name, resource);
		}

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)]->Create(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN, rtVec, dsTexture);
	}

	// Shadow Group
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_SHADOW_GROUP_MEMBER_COUNT);

		rtVec[0].target = GET_SINGLE(Resources)->CreateTexture(L"ShadowTarget",
			DXGI_FORMAT_R32_FLOAT, 8192, 8192,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		shared_ptr<Texture> shadowDepthTexture = GET_SINGLE(Resources)->CreateTexture(L"ShadowDepthStencil",
			DXGI_FORMAT_D32_FLOAT, 8192, 8192,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SHADOW)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SHADOW)]->Create(RENDER_TARGET_GROUP_TYPE::SHADOW, rtVec, shadowDepthTexture);
	}

	// Deferred Group
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT);

		rtVec[0].target = GET_SINGLE(Resources)->CreateTexture(L"PositionTarget",
			DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		rtVec[1].target = GET_SINGLE(Resources)->CreateTexture(L"NormalTarget",
			DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		rtVec[2].target = GET_SINGLE(Resources)->CreateTexture(L"DiffuseTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)]->Create(RENDER_TARGET_GROUP_TYPE::G_BUFFER, rtVec, dsTexture);
	}
	
	// Lighting Group
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT);

		rtVec[0].target = GET_SINGLE(Resources)->CreateTexture(L"DiffuseLightTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		rtVec[1].target = GET_SINGLE(Resources)->CreateTexture(L"SpecularLightTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::LIGHTING)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::LIGHTING)]->Create(RENDER_TARGET_GROUP_TYPE::LIGHTING, rtVec, dsTexture);
	}
}

void Engine::InitializePhysics()
{
	_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, _allocator, _errorCallback);
	if (!_foundation) {
		std::cerr << "PxCreateFoundation failed!" << std::endl;
		return;
	}

	_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, physx::PxTolerancesScale(), true);
	if (!_physics) {
		std::cerr << "PxCreatePhysics failed!" << std::endl;
		return;
	}

	_cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(MAX_NUM_PX_THREADS);
	if (!_cpuDispatcher) {
		std::cerr << "PxDefaultCpuDispatcherCreate failed!" << std::endl;
		return;
	}

	physx::PxSceneDesc sceneDesc(_physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = _cpuDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	sceneDesc.kineKineFilteringMode = physx::PxPairFilteringMode::eKEEP;
	sceneDesc.staticKineFilteringMode = physx::PxPairFilteringMode::eKEEP;
	_scene = _physics->createScene(sceneDesc);
	if (!_scene) {
		std::cerr << "createScene failed!" << std::endl;
		return;
	}

	// 디버깅
	//_pvdSceneClient = _scene->getScenePvdClient();
	//_pvdSceneClient->setScenePvdFlags(
	//	physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS | 
	//	physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS | 
	//	physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES);

	_controllerManager = PxCreateControllerManager(*_scene);
}

void Engine::UpdatePhysics()
{
	float deltaTime = GET_SINGLE(Timer)->GetDeltaTime();
	_scene->simulate(deltaTime);
	_scene->fetchResults(true);
}
