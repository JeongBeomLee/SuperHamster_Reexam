#pragma once
#include "PxPhysicsAPI.h"
#include <random>
#include <mutex>
#include <vector>

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

enum class PLAYER_GUN
{
    DEFAULT,
    LASER,
    MAGNETIC,

    END
};

enum class PLAYER_STATE
{
    FIRE = 3,
    GETUP = 4,
    IDLE = 5,
    HIT = 6,
    RUN = 8,
    ROLL = 9,
    AIM = 11,

    END
};

class GameLogic
{
public:
    GameLogic();
    ~GameLogic();

    void InitializePhysics();
    void LoadMap(const char* filename);
    void Update(float deltaTime);

    void InitPlayersForGameStart();
    bool MovePlayer(uint32_t playerId, const physx::PxVec3& moveDir);
    bool CheckCollision(uint32_t playerId, uint32_t targetId);
    void ApplyGravity(float deltaTime);
    void AddActor(physx::PxActor* actor);
    
    void StartGame();

    struct AttackResult
    {
        bool hit;
        uint32_t targetId;
        int damage;
    };
    AttackResult ProcessAttack(uint32_t playerId, uint32_t attackType);

    bool ProcessInteraction(uint32_t playerId, uint32_t targetId, uint32_t interactionType);
	void UpdatePlayerState(uint32_t playerId, PLAYER_STATE newState) { _players[playerId].state = newState; }
	PLAYER_STATE GetPlayerState(uint32_t playerId) const { return _players[playerId].state; }

private:
    const int MAX_NUM_PX_THREADS = 4;  // 물리 시뮬레이션 스레드 개수
    const float FIXED_TIME_STEP = 1.0f / 60.0f;  // 물리 시뮬레이션 고정 시간 간격

    const float GRAVITY = -9.81f;
    const float MOVE_SPEED = 10.0f;
    const float ROLL_SPEED = 15.0f;

    const float PLAYER_ATTACK_RANGE = 50.0f;  // 플레이어 공격 범위

    // 플레이어 상태 관리
    struct PlayerInfo
    {
        float x, y, z;
        float health;
        PLAYER_GUN gun;
        PLAYER_STATE state;
    };
    PlayerInfo _players[2];

    std::mt19937 _rng;  // 난수 생성기

    physx::PxDefaultAllocator       _allocator;
    physx::PxDefaultErrorCallback   _errorCallback;
    physx::PxDefaultCpuDispatcher*  _cpuDispatcher      = nullptr;
    physx::PxFoundation*            _foundation         = nullptr;
    physx::PxPhysics*               _physics            = nullptr;
    physx::PxScene*                 _scene              = nullptr;
    physx::PxControllerManager*     _controllerManager  = nullptr;
    physx::PxTriangleMesh*          _mapMesh            = nullptr;

    physx::PxPvd* _pvd = nullptr;
    physx::PxPvdTransport* _pvdTransport = nullptr;
    physx::PxPvdSceneClient* _pvdSceneClient = nullptr;

    struct PlayerPhysicsState
    {
        physx::PxController* controller;
        physx::PxVec3 moveDirection;
    };
    PlayerPhysicsState _playerPhysics[2];

    friend class GameServer;
};