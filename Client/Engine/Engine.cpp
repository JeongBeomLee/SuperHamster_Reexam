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
#include "CharacterMovement.h"
#include "PlayerManager.h"
#include "ProjectileManager.h"
#include "TeleportSystem.h"
#include "SoundSystem.h"
#include "EventManager.h"
#include "NetworkManager.h"
#include "RemoteProjectileManager.h"

void Engine::Init(const WindowInfo& info)
{
	// 로거 초기화
	Logger::Instance().AddOutput(std::make_unique<DebugOutput>());
	Logger::Instance().Info("Engine 초기화 시작");

	_window = info;	

	// 그려질 화면 크기를 설정
	_viewport = { 0, 0, static_cast<FLOAT>(info.width), static_cast<FLOAT>(info.height), 0.0f, 1.0f };
	_scissorRect = CD3DX12_RECT(0, 0, info.width, info.height);

	_device->Init();
	_graphicsCmdQueue->Init(_device->GetDevice(), _swapChain);
	_computeCmdQueue->Init(_device->GetDevice());
	_swapChain->Init(info, _device->GetDevice(), _device->GetDXGI(), _graphicsCmdQueue->GetCmdQueue());
	_rootSignature->Init();
	_graphicsDescHeap->Init(MAX_GRAPHICS_DESC_HEAP_COUNT);
	_computeDescHeap->Init();
	if (!_physicsEngine->Initialize()) {
		Logger::Instance().Error("물리 엔진 초기화 실패");
		return;
	}

	CreateConstantBuffer(CBV_REGISTER::b0, sizeof(LightParams), 1);
	CreateConstantBuffer(CBV_REGISTER::b1, sizeof(TransformParams), MAX_TRANSFORM_MATRICES);
	CreateConstantBuffer(CBV_REGISTER::b2, sizeof(MaterialParams), MAX_MATERIALS);

	CreateRenderTargetGroups();

	RegisterEventHandlers();

	GET_SINGLE(Input)->Init(info.hwnd);
	GET_SINGLE(Timer)->Init();
	GET_SINGLE(Resources)->Init();
	GET_SINGLE(NetworkManager)->Init(SERVER_PORT);
}

void Engine::Update()
{
	// 1. 입력 및 시스템 업데이트
	GET_SINGLE(Input)->Update();
	GET_SINGLE(Timer)->Update();

	// 2. 이벤트 매니저 선처리
	GET_SINGLE(EventManager)->Update();

	// 3. 게임 로직 업데이트
	GET_SINGLE(SoundSystem)->Update();
	GET_SINGLE(PlayerManager)->Update();
	PHYSICS_ENGINE->Update();

	// 4. 이벤트 매니저 중간 처리
	GET_SINGLE(EventManager)->Update();

	// 5. 나머지 시스템 업데이트
	GET_SINGLE(TeleportSystem)->Update();
	GET_SINGLE(SceneManager)->Update();
	GET_SINGLE(ProjectileManager)->Update();
	GET_SINGLE(RemoteProjectileManager)->Update();
	GET_SINGLE(InstancingManager)->ClearBuffer();

	// 6. 이벤트 매니저 후처리
	GET_SINGLE(EventManager)->Update();

	Render();
	ShowFps();
}

void Engine::ToggleFullscreen()
{
	_window.windowed = !_window.windowed;
	bool fullscreen = !_window.windowed;

	if (fullscreen) {
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
	else {
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

void Engine::RegisterEventHandlers()
{
	// 이벤트 하나 추가할 때 마다 Engine, EventManager 수정 필요
	GET_SINGLE(EventManager)->Subscribe<Event::CollisionEvent>(
		Event::EventCallback<Event::CollisionEvent>(
			[this](const Event::CollisionEvent& event) {
				// 충돌 이벤트 처리
				Logger::Instance().Debug("'{}'와 '{}'가 충돌 발생. 위치: ({}, {}, {}), 노말: ({}, {}, {}), 충격량: ({})",
					event.actor1->getName(),
					event.actor2->getName(),
					event.position.x,
					event.position.y,
					event.position.z,
					event.normal.x,
					event.normal.y,
					event.normal.z,
					event.impulse);
			})
	);

	GET_SINGLE(EventManager)->Subscribe<Event::InputEvent>(
		Event::EventCallback<Event::InputEvent>(
			[this](const Event::InputEvent& event) {
				// 입력 이벤트 처리
				switch (event.type) {
				case Event::InputEvent::Type::KeyDown:
					Logger::Instance().Debug("키 눌림: {}", event.code);
					break;
				case Event::InputEvent::Type::KeyUp:
					Logger::Instance().Debug("키 뗌: {}", event.code);
					break;
				case Event::InputEvent::Type::MouseMove:
					Logger::Instance().Debug("마우스 이동: ({}, {})", event.x, event.y);
					break;
				case Event::InputEvent::Type::MouseButtonDown:
					Logger::Instance().Debug("마우스 버튼 눌림: {}", event.code);
					break;
				case Event::InputEvent::Type::MouseButtonUp:
					Logger::Instance().Debug("마우스 버튼 뗌: {}", event.code);
					break;
				}
			})
	);

	// 투사체 충돌 이벤트 구독
	GET_SINGLE(EventManager)->Subscribe<Event::ProjectileHitEvent>(
		Event::EventCallback<Event::ProjectileHitEvent>(
			[this](const Event::ProjectileHitEvent& event) {
				// 충돌 이펙트 재생
				GET_SINGLE(ProjectileManager)->PlayCollisionEffect(event.hitPosition);

				// 충돌 사운드 재생
				auto sound = GET_SINGLE(Resources)->Get<Sound>(L"LaserHit");
				if (sound) {
					GET_SINGLE(SoundSystem)->Play3D(sound, event.hitPosition);
				}
			})
	);

	// 상태 변경 이벤트 처리
	//GET_SINGLE(EventManager)->Subscribe<StateChangeEvent>(
	//	Event::EventCallback<StateChangeEvent>([this](const StateChangeEvent& event) {
	//		// 상태에 따른 사운드 재생, 아니면 네트워킹 기능에서도 활용 가능
	//		if (event.newState == PLAYER_STATE::ROLL) {
	//			auto sound = GET_SINGLE(Resources)->Get<Sound>(L"RollSound");
	//			if (sound) {
	//				Vec3 position = event.player->GetTransform()->GetLocalPosition();
	//				GET_SINGLE(SoundSystem)->Play3D(sound, position);
	//			}
	//		}
	//		})
	//);
}