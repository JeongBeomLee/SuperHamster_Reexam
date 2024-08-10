#include "GameLogic.h"
#include <iostream>

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
}

void GameLogic::InitializePhysics()
{
    _foundation = PxCreateFoundation(PX_PHYSICS_VERSION, _allocator, _errorCallback);
    _physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, physx::PxTolerancesScale());

    physx::PxSceneDesc sceneDesc(_physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    _scene = _physics->createScene(sceneDesc);

    _controllerManager = PxCreateControllerManager(*_scene);

    std::cout << "Physics initialized" << std::endl;
}

void GameLogic::LoadMap(const char* filename)
{
    // PxTriangleMesh�� ����Ͽ� �� ������ �ε�
    // �� �κ��� �� ���� ���Ŀ� ���� ������ �޶��� �� �ֽ��ϴ�.
}

void GameLogic::UpdatePhysics(float deltaTime)
{
    _scene->simulate(deltaTime);
    _scene->fetchResults(true);

    // �÷��̾� ��ġ ������Ʈ
    for (int i = 0; i < 2; ++i) {
        physx::PxExtendedVec3 position = _playerPhysics[i].controller->getPosition();
        _players[i].x = position.x;
        _players[i].y = position.y;
        _players[i].z = position.z;
    }
}

void GameLogic::InitializePlayerPosition(uint32_t playerId, physx::PxExtendedVec3& position, physx::PxVec3& direction)
{
    // ���� ���� �� �÷��̾��� �ʱ� ��ġ �� ���� ����
    position = physx::PxExtendedVec3(0, 0, 0);

    direction = physx::PxVec3(0, 0, 0);
    direction.normalize();

    physx::PxCapsuleControllerDesc desc;
    desc.position = position;
    desc.height = 50.0f; // �÷��̾� ����
    desc.radius = 25.0f; // �÷��̾� �ݰ�
    desc.material = _physics->createMaterial(0.5f, 0.5f, 0.1f);  // 1: ���� ������, 2: ���� ������, 3: �ݹ� ���

    _playerPhysics[playerId].controller = _controllerManager->createController(desc);
    _playerPhysics[playerId].controller->setUpDirection(physx::PxVec3(0, 1, 0));    // Y���� ���� ����
    _playerPhysics[playerId].moveDirection = direction;

    std::cout << "Player " << playerId << " initialized" << std::endl;
}

bool GameLogic::MovePlayer(uint32_t playerId, const physx::PxVec3& displacement)
{
    physx::PxControllerFilters filters;
    physx::PxControllerCollisionFlags collisionFlags =
        _playerPhysics[playerId].controller->move(displacement, 0.001f, fixedTimeStep, filters);

    // collisionFlags�� ����Ͽ� �浹 ó�� ���� ����
    return true;  // �̵� ���� ���� ��ȯ
}

bool GameLogic::CheckCollision(uint32_t playerId, uint32_t targetId)
{
    return true;  // ������ ����: �׻� �浹
}

void GameLogic::StartGame()
{
    // ���� ���� �� �ʱ�ȭ ����
    for (int i = 0; i < 2; ++i) {
        _players[i].health = 100.0f;  // �ʱ� ü�� ����
    }

    std::cout << "GameLogic: Game started" << std::endl;
}

GameLogic::AttackResult GameLogic::ProcessAttack(uint32_t playerId, uint32_t attackType)
{
    AttackResult result;
    result.hit = false;
    result.targetId = 0;
    result.damage = 0;

    // ������ ����: �÷��̾� 0�� �÷��̾� 1�� ����
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