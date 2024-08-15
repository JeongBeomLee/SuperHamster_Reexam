#include "GameLogic.h"
#include <iostream>
#include <fstream>

GameLogic::GameLogic() : _rng(std::random_device{}())
{
    InitializePhysics();
}

GameLogic::~GameLogic()
{
    _controllerManager->release();
    _scene->release();
    _physics->release();
    _foundation->release();

    _pvd->release();
    _pvdTransport->release();
}

void GameLogic::InitializePhysics()
{
    _foundation = PxCreateFoundation(PX_PHYSICS_VERSION, _allocator, _errorCallback);
    if (!_foundation) {
        std::cerr << "PxCreateFoundation failed!" << std::endl;
		return;
    }

#ifdef _DEBUG
    _pvd = PxCreatePvd(*_foundation);
    _pvdTransport = physx::PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
    _pvd->connect(*_pvdTransport, physx::PxPvdInstrumentationFlag::eALL);
    _physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, physx::PxTolerancesScale(), true, _pvd);
#else
    _physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, physx::PxTolerancesScale(), true);
#endif

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
    _scene = _physics->createScene(sceneDesc);
    if (!_scene) {
		std::cerr << "createScene failed!" << std::endl;
		return;
	}

#ifdef _DEBUG
    _pvdSceneClient = pxDefaultScene->getScenePvdClient();
    _pvdSceneClient->setScenePvdFlags(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS | PxPvdSceneFlag::eTRANSMIT_CONTACTS | PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES);
#endif // _DEBUG

    _controllerManager = PxCreateControllerManager(*_scene);

    std::cout << "Physics initialized" << std::endl;
}

void GameLogic::LoadMap(const char* filename)
{
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Failed to open file for reading: " << filename << std::endl;
        return;
    }

    std::vector<physx::PxVec3> vertices;
    std::vector<physx::PxU32> indices;

    // Vertex 정보 읽기
    physx::PxU32 vertexCount;
    inFile.read(reinterpret_cast<char*>(&vertexCount), sizeof(physx::PxU32));
    vertices.resize(vertexCount);
    inFile.read(reinterpret_cast<char*>(vertices.data()), vertexCount * sizeof(physx::PxVec3));

    // Index 정보 읽기
    physx::PxU32 indexCount;
    inFile.read(reinterpret_cast<char*>(&indexCount), sizeof(physx::PxU32));
    indices.resize(indexCount);
    inFile.read(reinterpret_cast<char*>(indices.data()), indexCount * sizeof(physx::PxU32));

    physx::PxCookingParams params(_physics->getTolerancesScale());
    params.meshPreprocessParams = physx::PxMeshPreprocessingFlags(physx::PxMeshPreprocessingFlag::eWELD_VERTICES);
    params.midphaseDesc = physx::PxMeshMidPhase::eBVH34;
    params.convexMeshCookingType = physx::PxConvexMeshCookingType::eQUICKHULL;
    params.buildGPUData = true;
    params.meshWeldTolerance = 0.001f;

    physx::PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = static_cast<physx::PxU32>(vertices.size());
    meshDesc.points.stride = sizeof(physx::PxVec3);
    meshDesc.points.data = vertices.data();
    meshDesc.triangles.count = static_cast<physx::PxU32>(indices.size() / 3);
    meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
    meshDesc.triangles.data = indices.data();

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
    physx::PxQuat rotX = physx::PxQuat(-physx::PxPiDivTwo, physx::PxVec3(1, 0, 0));
    physx::PxTransform transform(physx::PxVec3(0, 0, 0), rotX);
    if (!transform.isValid()) {
        std::cerr << "Invalid PxTransform detected." << std::endl;
        return;
    }

    triangleMeshActor->setGlobalPose(transform);
    triangleMeshActor->attachShape(*triangleMeshShape);
    AddActor(triangleMeshActor);

    // 메모리 해제
    triangleMeshShape->release();
    triangleMesh->release();
    inFile.close();

    std::cout << "Map loaded from " << filename << std::endl;
}

void GameLogic::Update(float deltaTime)
{
    _scene->lockWrite();

    ApplyGravity(deltaTime);
    _scene->simulate(deltaTime);
    _scene->fetchResults(true);

    _scene->unlockWrite();
}

void GameLogic::InitPlayersForGameStart()
{
    _scene->lockWrite();

    physx::PxCapsuleControllerDesc player1Desc;
    player1Desc.position = physx::PxExtendedVec3(-460.224, 500, 60.2587); // Player1 초기 위치 입력
    player1Desc.height = 50.0f; // 플레이어 높이
    player1Desc.radius = 25.0f; // 플레이어 반경
    player1Desc.clientID = 0;     // 클라이언트 ID 설정
    player1Desc.userData = &_players[0]; // 플레이어 정보 입력
    player1Desc.material = _physics->createMaterial(0.5f, 0.5f, 0.1f); // 1: 정지 마찰력, 2: 동적 마찰력, 3: 반발 계수
    player1Desc.upDirection = physx::PxVec3(0, 1, 0); // Y축을 위로 설정
    player1Desc.contactOffset = 0.1f; // 플레이어 이동 시 캡슐과의 거리 설정(땅과의 거리)
    player1Desc.climbingMode = physx::PxCapsuleClimbingMode::eEASY; // 플레이어 이동 시 오르기 모드 설정
    player1Desc.slopeLimit = cosf(physx::PxDegToRad(45.f));; // 플레이어 이동 시 올라갈 수 있는 경사 설정
    player1Desc.density = 10.0f; // 플레이어 이동 시 밀도 설정
    player1Desc.stepOffset = 0.5f;  // 플레이어 이동 시 올라갈 수 있는 높이 설정
    player1Desc.maxJumpHeight = 0.0f;  // 플레이어 이동 시 최대 점프 높이 설정 (점프 기능 없으니 0)
    player1Desc.invisibleWallHeight = 0.0f;  // 플레이어 이동 시 통과할 수 없는 벽 높이 설정
    player1Desc.nonWalkableMode = physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING; // 플레이어 이동 시 이동 불가능한 지형 설정
    player1Desc.reportCallback = nullptr;
    player1Desc.behaviorCallback = nullptr;

    _playerPhysics[0].controller = _controllerManager->createController(player1Desc);
    _playerPhysics[0].moveDirection = physx::PxVec3(0, 0, 1);

    physx::PxCapsuleControllerDesc player2Desc;
    player2Desc.position = physx::PxExtendedVec3(-400.224, 500, 60.2587);
    player2Desc.height = 50.0f;
    player2Desc.radius = 25.0f;
    player2Desc.clientID = 1;
    player2Desc.userData = &_players[1];
    player2Desc.material = _physics->createMaterial(0.5f, 0.5f, 0.1f);
    player2Desc.upDirection = physx::PxVec3(0, 1, 0);
    player2Desc.contactOffset = 0.1f;
    player2Desc.climbingMode = physx::PxCapsuleClimbingMode::eEASY;
    player2Desc.slopeLimit = cosf(physx::PxDegToRad(45.f));;
    player2Desc.density = 10.0f;
    player2Desc.stepOffset = 0.5f;
    player2Desc.maxJumpHeight = 0.0f;
    player2Desc.invisibleWallHeight = 0.0f;
    player2Desc.nonWalkableMode = physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING;
    player2Desc.reportCallback = nullptr;
    player2Desc.behaviorCallback = nullptr;

    _playerPhysics[1].controller = _controllerManager->createController(player2Desc);
    _playerPhysics[1].moveDirection = physx::PxVec3(0, 0, 1);

    _scene->unlockWrite();
}

bool GameLogic::MovePlayer(uint32_t playerId, const physx::PxVec3& moveDir)
{
    auto& controller = _playerPhysics[playerId].controller;
    if (!controller)
        return false;

    physx::PxVec3 movement = moveDir * MOVE_SPEED;
    _scene->lockWrite();
    physx::PxControllerCollisionFlags collisionFlags = controller->move(movement, 0.0001f, FIXED_TIME_STEP, physx::PxControllerFilters());
    _scene->unlockWrite();

    // 플레이어 위치 업데이트
    physx::PxExtendedVec3 position = controller->getPosition();
    _players[playerId].x = position.x;
    _players[playerId].y = position.y;
    _players[playerId].z = position.z;

    _playerPhysics[playerId].moveDirection = moveDir;

    return true;
}

bool GameLogic::CheckCollision(uint32_t playerId, uint32_t targetId)
{
    return true;  // 간단한 구현: 항상 충돌
}

void GameLogic::ApplyGravity(float deltaTime)
{
    for (auto& player : _playerPhysics) {
        if (player.controller != nullptr) {
            physx::PxVec3 gravity(0, GRAVITY * deltaTime, 0);
            player.controller->move(physx::PxVec3(0, GRAVITY, 0), 0.0001f, deltaTime, physx::PxControllerFilters());
        }
    }
}

void GameLogic::AddActor(physx::PxActor* actor)
{
    _scene->lockWrite();
    _scene->addActor(*actor);
    _scene->unlockWrite();
}

void GameLogic::StartGame()
{
    // 게임 시작 시 초기화 로직
    for (int i = 0; i < 2; ++i) {
        _players[i].health = 100.0f;  // 초기 체력 설정
        _players[i].gun = PLAYER_GUN::DEFAULT;  // 초기 무기 설정
    }

    // 1. 맵 로드 및 배치
    LoadMap("Stage1.bin");
    
    // 2. 플레이어 초기 위치 및 방향 설정
    InitPlayersForGameStart();
    
    // 3. 몬스터 초기 위치 및 방향 설정
    
    // 4. 상호작용 오브젝트 초기 위치 설정
    
    // 5. 기타 초기화 작업
    
    // 6. 게임 시작

    std::cout << "GameLogic: Game started" << std::endl;
}

GameLogic::AttackResult GameLogic::ProcessAttack(uint32_t playerId, uint32_t attackType)
{
    AttackResult result;
    result.hit = false;
    result.targetId = 0;
    result.damage = 0;

    // 예시: 플레이어 0이 플레이어 1을 공격
    if (playerId == 0 && CheckCollision(0, 1)) {
        result.hit = true;
        result.targetId = 1;

        // 공격 타입에 따른 데미지 계산 (간단한 예시)
        std::uniform_int_distribution<> damageDis(5, 20);
        result.damage = damageDis(_rng);

        // 대상 플레이어의 체력 감소
        _players[1].health -= result.damage;
        if (_players[1].health < 0) _players[1].health = 0;
    }

    return result;
}

bool GameLogic::ProcessInteraction(uint32_t playerId, uint32_t targetId, uint32_t interactionType)
{
    // 간단한 상호작용 로직 (예: 아이템 획득)
    // 실제 게임에서는 더 복잡한 로직이 필요할 수 있습니다.
    return true;
}