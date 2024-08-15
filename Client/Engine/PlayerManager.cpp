#include "pch.h"
#include "PlayerManager.h"
#include "Timer.h"

Player* PlayerManager::CreatePlayer(uint32_t playerId, bool isLocal)
{
    auto player = std::make_unique<Player>(playerId, isLocal);
    auto result = player.get();
    _players[playerId] = std::move(player);
    return result;
}

Player* PlayerManager::CreatePlayer(uint32_t playerId, bool isLocal, std::shared_ptr<GameObject> gameObject)
{
    auto player = std::make_unique<Player>(playerId, isLocal, gameObject);
	auto result = player.get();
	_players[playerId] = std::move(player);
	return result;
}

Player* PlayerManager::GetPlayer(uint32_t playerId)
{
    auto it = _players.find(playerId);
    return it != _players.end() ? it->second.get() : nullptr;
}

Player* PlayerManager::GetLocalPlayer()
{
    for (auto& pair : _players)
    {
        if (pair.second->IsLocal())
        {
            return pair.second.get();
        }
    }
    return nullptr;
}

void PlayerManager::Update()
{
    float deltaTime = DELTA_TIME;
    for (auto& pair : _players)
    {
        pair.second->Update(deltaTime);
    }
}