#pragma once
#include "Player.h"

class PlayerManager
{
public:
    DECLARE_SINGLE(PlayerManager)

    Player* CreatePlayer(uint32_t playerId, std::shared_ptr<GameObject> gameObject);
    Player* GetPlayer(uint32_t playerId);
	const std::unordered_map<uint32_t, std::unique_ptr<Player>>& GetPlayers() const { return _players; }

    void Update();

private:
    std::unordered_map<uint32_t, std::unique_ptr<Player>> _players;
};