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
    // PxTriangleMesh를 사용하여 맵 데이터 로드
    // 이 부분은 맵 파일 형식에 따라 구현이 달라질 수 있습니다.
}

void GameLogic::UpdatePhysics(float deltaTime)
{
    _scene->simulate(deltaTime);
    _scene->fetchResults(true);

    // 플레이어 위치 업데이트
    for (int i = 0; i < 2; ++i) {
        physx::PxExtendedVec3 position = _playerPhysics[i].controller->getPosition();
        _players[i].x = position.x;
        _players[i].y = position.y;
        _players[i].z = position.z;
    }
}

void GameLogic::InitializePlayerPosition(uint32_t playerId, physx::PxExtendedVec3& position, physx::PxVec3& direction)
{
    // 게임 시작 시 플레이어의 초기 위치 및 방향 설정
    position = physx::PxExtendedVec3(0, 0, 0);

    direction = physx::PxVec3(0, 0, 0);
    direction.normalize();

    physx::PxCapsuleControllerDesc desc;
    desc.position = position;
    desc.height = 50.0f; // 플레이어 높이
    desc.radius = 25.0f; // 플레이어 반경
    desc.material = _physics->createMaterial(0.5f, 0.5f, 0.1f);  // 1: 정지 마찰력, 2: 동적 마찰력, 3: 반발 계수

    _playerPhysics[playerId].controller = _controllerManager->createController(desc);
    _playerPhysics[playerId].controller->setUpDirection(physx::PxVec3(0, 1, 0));    // Y축을 위로 설정
    _playerPhysics[playerId].moveDirection = direction;

    std::cout << "Player " << playerId << " initialized" << std::endl;
}

bool GameLogic::MovePlayer(uint32_t playerId, const physx::PxVec3& displacement)
{
    physx::PxControllerFilters filters;
    physx::PxControllerCollisionFlags collisionFlags =
        _playerPhysics[playerId].controller->move(displacement, 0.001f, fixedTimeStep, filters);

    // collisionFlags를 사용하여 충돌 처리 로직 구현
    return true;  // 이동 성공 여부 반환
}

bool GameLogic::CheckCollision(uint32_t playerId, uint32_t targetId)
{
    return true;  // 간단한 구현: 항상 충돌
}

void GameLogic::StartGame()
{
    // 게임 시작 시 초기화 로직
    for (int i = 0; i < 2; ++i) {
        _players[i].health = 100.0f;  // 초기 체력 설정
    }

    std::cout << "GameLogic: Game started" << std::endl;
}

GameLogic::AttackResult GameLogic::ProcessAttack(uint32_t playerId, uint32_t attackType)
{
    AttackResult result;
    result.hit = false;
    result.targetId = 0;
    result.damage = 0;

    // 간단한 구현: 플레이어 0이 플레이어 1을 공격
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