#include "pch.h"
#include "PlayerManager.h"
#include "Timer.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneManager.h"
#include "PlayerHealthBar.h"
#include "EventManager.h"

PlayerManager::PlayerManager()
{
    // 네트워크 입력 이벤트 구독
    m_networkInputEventId = GET_SINGLE(EventManager)->Subscribe<Event::NetworkInputEvent>(
        Event::EventCallback<Event::NetworkInputEvent>(
            [this](const Event::NetworkInputEvent& event) {
                OnNetworkInput(event);
            })
    );
}

PlayerManager::~PlayerManager()
{
	GET_SINGLE(EventManager)->Unsubscribe<Event::NetworkInputEvent>(m_networkInputEventId);
}

Player* PlayerManager::CreatePlayer(uint32_t playerId, std::shared_ptr<GameObject> gameObject)
{
    auto player = std::make_unique<Player>(playerId, gameObject);

    gameObject->SetName(L"Player" + to_wstring(playerId));
	Logger::Instance().Info("플레이어 생성됨. ID: {}", playerId);

    gameObject->SetCheckFrustum(false);
    gameObject->SetStatic(false);

    if (playerId == 0) {
        gameObject->GetTransform()->SetLocalPosition(Vec3(-60.224f, 200.f, 60.2587f));
	}
	else {
		gameObject->GetTransform()->SetLocalPosition(Vec3(60.224f, 200.f, 60.2587f));
	}
    
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

    if (player->IsLocalPlayer()) {
        gameObject->AddComponent(std::make_shared<PlayerHealthBar>());
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

void PlayerManager::OnNetworkInput(const Event::NetworkInputEvent& event)
{
    auto player = GetPlayer(event.inputData.playerID);
    if (!player || player->IsLocalPlayer()) {
        return;
    }

    // 위치 동기화
    auto controller = player->GetGameObject()->GetCharacterController();
	controller->Teleport(Vec3(event.inputData.position.x, event.inputData.position.y + 100.f, event.inputData.position.z));
    player->SetState(event.inputData.currentState);

    // 네트워크 입력 처리
    player->ProcessNetworkInput(event.inputData);
}

void PlayerManager::Update()
{
    float deltaTime = DELTA_TIME;
    for (auto& pair : _players) {
        pair.second->Update(deltaTime);
    }
}