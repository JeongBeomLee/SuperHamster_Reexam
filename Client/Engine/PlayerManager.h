#pragma once
#include "Player.h"

class PlayerManager
{
private:
	PlayerManager();
	~PlayerManager();
public:
	static PlayerManager* GetInstance()
	{
		static PlayerManager instance;
		return &instance;
	}

public:
    Player* CreatePlayer(uint32_t playerId, std::shared_ptr<GameObject> gameObject);
    Player* GetPlayer(uint32_t playerId);
	const std::unordered_map<uint32_t, std::unique_ptr<Player>>& GetPlayers() const { return _players; }
	void OnNetworkInput(const Event::NetworkInputEvent& event);
    void Update();

private:
    std::unordered_map<uint32_t, std::unique_ptr<Player>> _players;
    size_t m_networkInputEventId;
};