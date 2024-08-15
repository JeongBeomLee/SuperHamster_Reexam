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

    // Vertex ���� �б�
    physx::PxU32 vertexCount;
    inFile.read(reinterpret_cast<char*>(&vertexCount), sizeof(physx::PxU32));
    vertices.resize(vertexCount);
    inFile.read(reinterpret_cast<char*>(vertices.data()), vertexCount * sizeof(physx::PxVec3));

    // Index ���� �б�
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

    // PxTriangleMeshDesc�� ����ȭ�ϱ� ���� �޸� ��Ʈ�� ����
    physx::PxDefaultMemoryOutputStream writeBuffer;
    physx::PxTriangleMeshCookingResult::Enum result;
    bool status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);
    if (!status) {
        std::cerr << "Failed to cook triangle mesh." << std::endl;
    }

    if (writeBuffer.getSize() == 0) {
        std::cerr << "WriteBuffer is empty." << std::endl;
    }

    // ����ȭ�� �����͸� PxInputStream���� ��ȯ
    physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    physx::PxTriangleMesh* triangleMesh = _physics->createTriangleMesh(readBuffer);

    physx::PxMeshScale meshScale(physx::PxVec3(1, 1, 1));
    physx::PxTriangleMeshGeometry triangleMeshGeometry(triangleMesh, meshScale);
    physx::PxRigidStatic* triangleMeshActor = _physics->createRigidStatic(physx::PxTransform(physx::PxVec3(0, 0, 0)));
    physx::PxShape* triangleMeshShape = _physics->createShape(triangleMeshGeometry, *_physics->createMaterial(0.5f, 0.5f, 0.1f));

    // x�� ���� -90�� ȸ��
    physx::PxQuat rotX = physx::PxQuat(-physx::PxPiDivTwo, physx::PxVec3(1, 0, 0));
    physx::PxTransform transform(physx::PxVec3(0, 0, 0), rotX);
    if (!transform.isValid()) {
        std::cerr << "Invalid PxTransform detected." << std::endl;
        return;
    }

    triangleMeshActor->setGlobalPose(transform);
    triangleMeshActor->attachShape(*triangleMeshShape);
    AddActor(triangleMeshActor);

    // �޸� ����
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
    player1Desc.position = physx::PxExtendedVec3(-460.224, 500, 60.2587); // Player1 �ʱ� ��ġ �Է�
    player1Desc.height = 50.0f; // �÷��̾� ����
    player1Desc.radius = 25.0f; // �÷��̾� �ݰ�
    player1Desc.clientID = 0;     // Ŭ���̾�Ʈ ID ����
    player1Desc.userData = &_players[0]; // �÷��̾� ���� �Է�
    player1Desc.material = _physics->createMaterial(0.5f, 0.5f, 0.1f); // 1: ���� ������, 2: ���� ������, 3: �ݹ� ���
    player1Desc.upDirection = physx::PxVec3(0, 1, 0); // Y���� ���� ����
    player1Desc.contactOffset = 0.1f; // �÷��̾� �̵� �� ĸ������ �Ÿ� ����(������ �Ÿ�)
    player1Desc.climbingMode = physx::PxCapsuleClimbingMode::eEASY; // �÷��̾� �̵� �� ������ ��� ����
    player1Desc.slopeLimit = cosf(physx::PxDegToRad(45.f));; // �÷��̾� �̵� �� �ö� �� �ִ� ��� ����
    player1Desc.density = 10.0f; // �÷��̾� �̵� �� �е� ����
    player1Desc.stepOffset = 0.5f;  // �÷��̾� �̵� �� �ö� �� �ִ� ���� ����
    player1Desc.maxJumpHeight = 0.0f;  // �÷��̾� �̵� �� �ִ� ���� ���� ���� (���� ��� ������ 0)
    player1Desc.invisibleWallHeight = 0.0f;  // �÷��̾� �̵� �� ����� �� ���� �� ���� ����
    player1Desc.nonWalkableMode = physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING; // �÷��̾� �̵� �� �̵� �Ұ����� ���� ����
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

    // �÷��̾� ��ġ ������Ʈ
    physx::PxExtendedVec3 position = controller->getPosition();
    _players[playerId].x = position.x;
    _players[playerId].y = position.y;
    _players[playerId].z = position.z;

    _playerPhysics[playerId].moveDirection = moveDir;

    return true;
}

bool GameLogic::CheckCollision(uint32_t playerId, uint32_t targetId)
{
    return true;  // ������ ����: �׻� �浹
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
    // ���� ���� �� �ʱ�ȭ ����
    for (int i = 0; i < 2; ++i) {
        _players[i].health = 100.0f;  // �ʱ� ü�� ����
        _players[i].gun = PLAYER_GUN::DEFAULT;  // �ʱ� ���� ����
    }

    // 1. �� �ε� �� ��ġ
    LoadMap("Stage1.bin");
    
    // 2. �÷��̾� �ʱ� ��ġ �� ���� ����
    InitPlayersForGameStart();
    
    // 3. ���� �ʱ� ��ġ �� ���� ����
    
    // 4. ��ȣ�ۿ� ������Ʈ �ʱ� ��ġ ����
    
    // 5. ��Ÿ �ʱ�ȭ �۾�
    
    // 6. ���� ����

    std::cout << "GameLogic: Game started" << std::endl;
}

GameLogic::AttackResult GameLogic::ProcessAttack(uint32_t playerId, uint32_t attackType)
{
    AttackResult result;
    result.hit = false;
    result.targetId = 0;
    result.damage = 0;

    // ����: �÷��̾� 0�� �÷��̾� 1�� ����
    if (playerId == 0 && CheckCollision(0, 1)) {
        result.hit = true;
        result.targetId = 1;

        // ���� Ÿ�Կ� ���� ������ ��� (������ ����)
        std::uniform_int_distribution<> damageDis(5, 20);
        result.damage = damageDis(_rng);

        // ��� �÷��̾��� ü�� ����
        _players[1].health -= result.damage;
        if (_players[1].health < 0) _players[1].health = 0;
    }

    return result;
}

bool GameLogic::ProcessInteraction(uint32_t playerId, uint32_t targetId, uint32_t interactionType)
{
    // ������ ��ȣ�ۿ� ���� (��: ������ ȹ��)
    // ���� ���ӿ����� �� ������ ������ �ʿ��� �� �ֽ��ϴ�.
    return true;
}