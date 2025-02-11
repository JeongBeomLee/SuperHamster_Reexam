#include "pch.h"
#include "PlayerManager.h"
#include "Timer.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneManager.h"

Player* PlayerManager::CreatePlayer(uint32_t playerId, std::shared_ptr<GameObject> gameObject)
{
    auto player = std::make_unique<Player>(playerId, gameObject);

    gameObject->SetName(L"Player" + to_wstring(playerId));
	Logger::Instance().Info("플레이어 생성됨. ID: {}", playerId);

    gameObject->SetCheckFrustum(false);
    gameObject->SetStatic(false);
    gameObject->GetTransform()->SetLocalPosition(Vec3(-60.224f, 200.f, 60.2587f));
    gameObject->GetTransform()->SetLocalRotation(Vec3(0.f, 0.f, 0.f));
    gameObject->GetTransform()->SetLocalScale(Vec3(75.f, 75.f, 75.f));

    // CharacterController 초기화
    auto controller = gameObject->GetCharacterController();
    if (controller) {
		controller->SetCollisionGroup(CollisionGroup::Player);
        controller->SetCollisionMask(
            CollisionGroup::Default | CollisionGroup::Ground |
            CollisionGroup::Obstacle | CollisionGroup::Player |
            CollisionGroup::Enemy | CollisionGroup::Trigger);
		controller->SetRadius(40.f);
		controller->SetHeight(150.f);
        controller->Initialize();
    }

	auto result = player.get();
	_players[playerId] = std::move(player);
	return result;
}

Player* PlayerManager::GetPlayer(uint32_t playerId)
{
    auto it = _players.find(playerId);
    return it != _players.end() ? it->second.get() : nullptr;
}

void PlayerManager::Update()
{
    float deltaTime = DELTA_TIME;
    for (auto& pair : _players)
    {
        pair.second->Update(deltaTime);
    }
}