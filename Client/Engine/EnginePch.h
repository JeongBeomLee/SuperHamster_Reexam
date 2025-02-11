#pragma once
// std::byte 사용하지 않음
#define _HAS_STD_BYTE 0

// boost
#include "boost/asio.hpp"
using boost::asio::ip::tcp;

// STD
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <unordered_map>
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <fstream>
#include <format>
#include <filesystem>
namespace fs = std::filesystem;
using namespace std;

// DirectX
#include <d3d12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include "d3dx12.h"
#include "SimpleMath.h"
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

// DirectXTex
#include "DirectXTex.h"
#include "DirectXTex.inl"
#ifdef _DEBUG
	#pragma comment(lib, "DirectXTex_debug.lib")
#else
	#pragma comment(lib, "DirectXTex.lib")
#endif

// FBX SDK
#include "fbxsdk.h"
#ifdef _DEBUG
	#pragma comment(lib, "libfbxsdk-md.lib")
	#pragma comment(lib, "libxml2-md.lib")
	#pragma comment(lib, "zlib-md.lib")
#else
	#pragma comment(lib, "libfbxsdk-md.lib")
	#pragma comment(lib, "libxml2-md.lib")
	#pragma comment(lib, "zlib-md.lib")
#endif

// 물리 엔진
#include "PxPhysicsAPI.h"
using namespace physx;
#ifdef _DEBUG
	#pragma comment(lib, "PhysX_64.lib")
	#pragma comment(lib, "PhysXCommon_64.lib")
	#pragma comment(lib, "PhysXTask_static_64")
	#pragma comment(lib, "PhysXCooking_64.lib")
	#pragma comment(lib, "PhysXVehicle_static_64")
	#pragma comment(lib, "PhysXFoundation_64.lib")
	#pragma comment(lib, "PhysXVehicle2_static_64")
	#pragma comment(lib, "PhysXPvdSDK_static_64.lib")
	#pragma comment(lib, "PhysXExtensions_static_64.lib")
	#pragma comment(lib, "PhysXCharacterKinematic_static_64.lib")

	#pragma comment(lib, "PVDRuntime_64")
	#pragma comment(lib, "LowLevel_static_64")
	#pragma comment(lib, "SceneQuery_static_64")
	#pragma comment(lib, "LowLevelAABB_static_64")
	#pragma comment(lib, "LowLevelDynamics_static_64")
	#pragma comment(lib, "SimulationController_static_64")
#else
	#pragma comment(lib, "PhysX_64.lib")
	#pragma comment(lib, "PhysXCommon_64.lib")
	#pragma comment(lib, "PhysXTask_static_64")
	#pragma comment(lib, "PhysXCooking_64.lib")
	#pragma comment(lib, "PhysXVehicle_static_64")
	#pragma comment(lib, "PhysXFoundation_64.lib")
	#pragma comment(lib, "PhysXVehicle2_static_64")
	#pragma comment(lib, "PhysXPvdSDK_static_64.lib")
	#pragma comment(lib, "PhysXExtensions_static_64.lib")
	#pragma comment(lib, "PhysXCharacterKinematic_static_64.lib")

	#pragma comment(lib, "PVDRuntime_64")
	#pragma comment(lib, "LowLevel_static_64")
	#pragma comment(lib, "SceneQuery_static_64")
	#pragma comment(lib, "LowLevelAABB_static_64")
	#pragma comment(lib, "LowLevelDynamics_static_64")
	#pragma comment(lib, "SimulationController_static_64")
#endif

// FMOD
#include "fmod.hpp"
#include <fmod_errors.h>
#ifdef _DEBUG
	#pragma comment(lib, "fmodL_vc.lib")
#else
	#pragma comment(lib, "fmod_vc.lib")
#endif

// Tool
#include "Logger.h"

// 각종 typedef
using int8		= __int8;
using int16		= __int16;
using int32		= __int32;
using int64		= __int64;
using uint8		= unsigned __int8;
using uint16	= unsigned __int16;
using uint32	= unsigned __int32;
using uint64	= unsigned __int64;
using Vec2		= DirectX::SimpleMath::Vector2;
using Vec3		= DirectX::SimpleMath::Vector3;
using Vec4		= DirectX::SimpleMath::Vector4;
using Matrix	= DirectX::SimpleMath::Matrix;

enum class CBV_REGISTER : uint8
{
	b0,
	b1,
	b2,
	b3,
	b4,

	END
};

enum class SRV_REGISTER : uint8
{
	t0 = static_cast<uint8>(CBV_REGISTER::END),
	t1,
	t2,
	t3,
	t4,
	t5,
	t6,
	t7,
	t8,
	t9,

	END
};

enum class UAV_REGISTER : uint8
{
	u0 = static_cast<uint8>(SRV_REGISTER::END),
	u1,
	u2,
	u3,
	u4,

	END,
};

enum
{
	SWAP_CHAIN_BUFFER_COUNT = 2,
	CBV_REGISTER_COUNT = CBV_REGISTER::END,
	SRV_REGISTER_COUNT = static_cast<uint8>(SRV_REGISTER::END) - static_cast<uint8>(CBV_REGISTER_COUNT),
	CBV_SRV_REGISTER_COUNT = static_cast<uint8>(CBV_REGISTER_COUNT) + static_cast<uint8>(SRV_REGISTER_COUNT),
	UAV_REGISTER_COUNT = static_cast<uint8>(UAV_REGISTER::END) - CBV_SRV_REGISTER_COUNT,
	TOTAL_REGISTER_COUNT = CBV_SRV_REGISTER_COUNT + UAV_REGISTER_COUNT
};

struct WindowInfo
{
	HWND	hwnd; // 출력 윈도우
	int32	width; // 너비
	int32	height; // 높이
	bool	windowed; // 창모드 or 전체화면
};

struct Vertex
{
	Vertex() {}

	Vertex(Vec3 p, Vec2 u, Vec3 n, Vec3 t)
		: pos(p), uv(u), normal(n), tangent(t)
	{
	}

	Vec3 pos;
	Vec2 uv;
	Vec3 normal;
	Vec3 tangent;
	Vec4 weights;
	Vec4 indices;
};

#define DECLARE_SINGLE(type)		\
private:							\
	type() {}						\
	~type() {}						\
public:								\
	static type* GetInstance()		\
	{								\
		static type instance;		\
		return &instance;			\
	}								\

#define GET_SINGLE(type)	type::GetInstance()

#define DEVICE				GEngine->GetDevice()->GetDevice()
#define GRAPHICS_CMD_LIST	GEngine->GetGraphicsCmdQueue()->GetGraphicsCmdList()
#define RESOURCE_CMD_LIST	GEngine->GetGraphicsCmdQueue()->GetResourceCmdList()
#define COMPUTE_CMD_LIST	GEngine->GetComputeCmdQueue()->GetComputeCmdList()

#define GRAPHICS_ROOT_SIGNATURE		GEngine->GetRootSignature()->GetGraphicsRootSignature()
#define COMPUTE_ROOT_SIGNATURE		GEngine->GetRootSignature()->GetComputeRootSignature()

#define PHYSICS_ENGINE 	GEngine->GetPhysicsEngine()

#define INPUT				GET_SINGLE(Input)
#define DELTA_TIME			GET_SINGLE(Timer)->GetDeltaTime()

#define CONST_BUFFER(type)	GEngine->GetConstantBuffer(type)

constexpr uint32 MAX_NUM_PX_THREADS = 2;

struct TransformParams
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProjection;
	Matrix matWV;
	Matrix matWVP;
	Matrix matViewInv;
};

struct AnimFrameParams
{
	Vec4	scale;
	Vec4	rotation; // Quaternion
	Vec4	translation;
};

// 물리 객체의 타입을 구분하기 위한 열거형 클래스
enum class PhysicsObjectType {
	STATIC,
	DYNAMIC,
	KINEMATIC
};

extern unique_ptr<class Engine> GEngine;

// Utils
wstring s2ws(const string& s);
string ws2s(const wstring& s);
float repeat(float t, float length);
float ScalarLerpAngle(float a, float b, float t);
void QuaternionToEulerAngles(FXMVECTOR q, XMFLOAT3* pEuler);