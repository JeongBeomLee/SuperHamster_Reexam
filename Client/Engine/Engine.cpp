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
	// �ΰ� �ʱ�ȭ
	Logger::Instance().AddOutput(std::make_unique<DebugOutput>());
	Logger::Instance().Info("Engine �ʱ�ȭ ����");

	_window = info;	

	// �׷��� ȭ�� ũ�⸦ ����
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
		Logger::Instance().Error("���� ���� �ʱ�ȭ ����");
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
	// 1. �Է� �� �ý��� ������Ʈ
	GET_SINGLE(Input)->Update();
	GET_SINGLE(Timer)->Update();

	// 2. �̺�Ʈ �Ŵ��� ��ó��
	GET_SINGLE(EventManager)->Update();

	// 3. ���� ���� ������Ʈ
	GET_SINGLE(SoundSystem)->Update();
	GET_SINGLE(PlayerManager)->Update();
	PHYSICS_ENGINE->Update();

	// 4. �̺�Ʈ �Ŵ��� �߰� ó��
	GET_SINGLE(EventManager)->Update();

	// 5. ������ �ý��� ������Ʈ
	GET_SINGLE(TeleportSystem)->Update();
	GET_SINGLE(SceneManager)->Update();
	GET_SINGLE(ProjectileManager)->Update();
	GET_SINGLE(RemoteProjectileManager)->Update();
	GET_SINGLE(InstancingManager)->ClearBuffer();

	// 6. �̺�Ʈ �Ŵ��� ��ó��
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

		// �����츦 ��ü ȭ�� ���� ��ȯ
		::SetWindowLongPtr(_window.hwnd, GWL_STYLE, WS_POPUP);
		::SetWindowPos(_window.hwnd, HWND_TOP, 0, 0, _window.width, _window.height, SWP_FRAMECHANGED);
		::ShowWindow(_window.hwnd, SW_MAXIMIZE);

		// ��ü ȭ�� ���� ��ȯ
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

		// �����츦 â ���� ��ȯ
		::SetWindowLongPtr(_window.hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		RECT rect = { 0, 0, _window.width, _window.height };
		::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
		::SetWindowPos(_window.hwnd, 0, 100, 100, rect.right - rect.left, rect.bottom - rect.top, 0);
		::ShowWindow(_window.hwnd, SW_SHOWNORMAL);

		// â ���� ��ȯ
		DXGI_MODE_DESC windowedMode = {};
		windowedMode.Width = _window.width;
		windowedMode.Height = _window.height;
		windowedMode.RefreshRate = { 60, 1 };
		windowedMode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		_swapChain->GetSwapChain()->ResizeTarget(&windowedMode);
	}

	// �׷��� ȭ�� ũ�⸦ ����
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
	// �̺�Ʈ �ϳ� �߰��� �� ���� Engine, EventManager ���� �ʿ�
	GET_SINGLE(EventManager)->Subscribe<Event::CollisionEvent>(
		Event::EventCallback<Event::CollisionEvent>(
			[this](const Event::CollisionEvent& event) {
				// �浹 �̺�Ʈ ó��
				Logger::Instance().Debug("'{}'�� '{}'�� �浹 �߻�. ��ġ: ({}, {}, {}), �븻: ({}, {}, {}), ��ݷ�: ({})",
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
				// �Է� �̺�Ʈ ó��
				switch (event.type) {
				case Event::InputEvent::Type::KeyDown:
					Logger::Instance().Debug("Ű ����: {}", event.code);
					break;
				case Event::InputEvent::Type::KeyUp:
					Logger::Instance().Debug("Ű ��: {}", event.code);
					break;
				case Event::InputEvent::Type::MouseMove:
					Logger::Instance().Debug("���콺 �̵�: ({}, {})", event.x, event.y);
					break;
				case Event::InputEvent::Type::MouseButtonDown:
					Logger::Instance().Debug("���콺 ��ư ����: {}", event.code);
					break;
				case Event::InputEvent::Type::MouseButtonUp:
					Logger::Instance().Debug("���콺 ��ư ��: {}", event.code);
					break;
				}
			})
	);

	// ����ü �浹 �̺�Ʈ ����
	GET_SINGLE(EventManager)->Subscribe<Event::ProjectileHitEvent>(
		Event::EventCallback<Event::ProjectileHitEvent>(
			[this](const Event::ProjectileHitEvent& event) {
				// �浹 ����Ʈ ���
				GET_SINGLE(ProjectileManager)->PlayCollisionEffect(event.hitPosition);

				// �浹 ���� ���
				auto sound = GET_SINGLE(Resources)->Get<Sound>(L"LaserHit");
				if (sound) {
					GET_SINGLE(SoundSystem)->Play3D(sound, event.hitPosition);
				}
			})
	);

	// ���� ���� �̺�Ʈ ó��
	//GET_SINGLE(EventManager)->Subscribe<StateChangeEvent>(
	//	Event::EventCallback<StateChangeEvent>([this](const StateChangeEvent& event) {
	//		// ���¿� ���� ���� ���, �ƴϸ� ��Ʈ��ŷ ��ɿ����� Ȱ�� ����
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