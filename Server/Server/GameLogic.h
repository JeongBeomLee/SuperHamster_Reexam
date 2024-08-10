#pragma once
#include "../../PhysX/include/PxPhysicsAPI.h"
#include <random>

class GameLogic
{
public:
    GameLogic();
    ~GameLogic();

    void InitializePhysics();
    void LoadMap(const char* filename);
    void UpdatePhysics(float deltaTime);

    void InitializePlayerPosition(uint32_t playerId, physx::PxExtendedVec3& position, physx::PxVec3& direction);
    bool MovePlayer(uint32_t playerId, const physx::PxVec3& displacement);
    bool CheckCollision(uint32_t playerId, uint32_t targetId);

    void StartGame();

    struct AttackResult
    {
        bool hit;
        uint32_t targetId;
        int damage;
    };
    AttackResult ProcessAttack(uint32_t playerId, uint32_t attackType);

    bool ProcessInteraction(uint32_t playerId, uint32_t targetId, uint32_t interactionType);

private:
    const float fixedTimeStep = 1.0f / 60.0f;  // 물리 시뮬레이션 고정 시간 간격
    const float PLAYER_ATTACK_RANGE = 50.0f;  // 플레이어 공격 범위

    // 게임 상태 관리
    struct PlayerState
    {
        float x, y, z;
        float health;
    };
    PlayerState _players[2];

    std::mt19937 _rng;  // 난수 생성기

    physx::PxDefaultAllocator      _allocator;
    physx::PxDefaultErrorCallback  _errorCallback;
    physx::PxFoundation* _foundation;
    physx::PxPhysics* _physics;
    physx::PxScene* _scene;
    physx::PxControllerManager* _controllerManager;
    physx::PxTriangleMesh* _mapMesh;

    struct PlayerPhysicsState
    {
        physx::PxController* controller;
        physx::PxVec3 moveDirection;
    };
    PlayerPhysicsState _playerPhysics[2];

    friend class GameServer;
};