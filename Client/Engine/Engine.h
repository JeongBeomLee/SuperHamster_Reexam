#pragma once

#include "Device.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "RootSignature.h"
#include "Mesh.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "TableDescriptorHeap.h"
#include "Texture.h"
#include "RenderTargetGroup.h"
#include "Timer.h"
#include "MeshData.h"
#include "PhysicsEngine.h"

class Engine
{
public:
	Engine();
	~Engine();

	void Init(const WindowInfo& info);
	void Update();
	void ToggleFullscreen();

public:
	const WindowInfo& GetWindow() { return _window; }
	shared_ptr<Device> GetDevice() { return _device; }
	shared_ptr<GraphicsCommandQueue> GetGraphicsCmdQueue() { return _graphicsCmdQueue; }
	shared_ptr<ComputeCommandQueue> GetComputeCmdQueue() { return _computeCmdQueue; }
	shared_ptr<SwapChain> GetSwapChain() { return _swapChain; }
	shared_ptr<RootSignature> GetRootSignature() { return _rootSignature; }
	shared_ptr<GraphicsDescriptorHeap> GetGraphicsDescHeap() { return _graphicsDescHeap; }
	shared_ptr<ComputeDescriptorHeap> GetComputeDescHeap() { return _computeDescHeap; }

	shared_ptr<ConstantBuffer> GetConstantBuffer(CONSTANT_BUFFER_TYPE type) { return _constantBuffers[static_cast<uint8>(type)]; }
	shared_ptr<RenderTargetGroup> GetRTGroup(RENDER_TARGET_GROUP_TYPE type) { return _rtGroups[static_cast<uint8>(type)]; }

	PhysicsEngine* GetPhysicsEngine() const { return _physicsEngine.get(); }

	int GetMyPlayerId() const { return _myPlayerId; }
	int GetOtherPlayerId() const { return 1 - _myPlayerId; }
	void SetMyPlayerId(int playerId) { _myPlayerId = playerId; }

	void LoadMapMeshForPhysics(const shared_ptr<MeshData>& meshData);
	void SaveMeshDataToBinary(const std::vector<physx::PxVec3>& vertices, const std::vector<physx::PxU32>& indices, const std::string& filePath);

public:
	void Render();
	void RenderBegin();
	void RenderEnd();

	void ResizeWindow(int32 width, int32 height);

private:
	void ShowFps();
	void CreateConstantBuffer(CBV_REGISTER reg, uint32 bufferSize, uint32 count);
	void CreateRenderTargetGroups();

private:
	// 그려질 화면 크기 관련
	WindowInfo		_window;
	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};

	const uint32 MAX_OBJECT_COUNT = 5000;
	const uint32 MAX_GRAPHICS_DESC_HEAP_COUNT = 1000; // 1000 * 14(CBV, SRV 개수) = 14000
	const uint32 MAX_TRANSFORM_MATRICES = MAX_OBJECT_COUNT; // 1개 오브젝트당 1개의 트랜스폼
	const uint32 MAX_MATERIALS = MAX_OBJECT_COUNT; // 1개 오브젝트당 1개의 머티리얼

	shared_ptr<Device> _device = make_shared<Device>();
	shared_ptr<GraphicsCommandQueue> _graphicsCmdQueue = make_shared<GraphicsCommandQueue>();
	shared_ptr<ComputeCommandQueue> _computeCmdQueue = make_shared<ComputeCommandQueue>();
	shared_ptr<SwapChain> _swapChain = make_shared<SwapChain>();
	shared_ptr<RootSignature> _rootSignature = make_shared<RootSignature>();
	shared_ptr<GraphicsDescriptorHeap> _graphicsDescHeap = make_shared<GraphicsDescriptorHeap>();
	shared_ptr<ComputeDescriptorHeap> _computeDescHeap = make_shared<ComputeDescriptorHeap>();
	std::unique_ptr<PhysicsEngine> _physicsEngine = std::make_unique<PhysicsEngine>();

	vector<shared_ptr<ConstantBuffer>> _constantBuffers;
	array<shared_ptr<RenderTargetGroup>, RENDER_TARGET_GROUP_COUNT> _rtGroups;

	int _myPlayerId = -1;
};

