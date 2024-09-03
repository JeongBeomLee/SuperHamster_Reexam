#pragma once
#include "PlayerStateMachine.h"

class GameObject;
class Animator;
class Player
{
public:
    Player(uint32_t playerId, bool isLocal);
    Player(uint32_t playerId, bool isLocal, std::shared_ptr<GameObject> gameObject);

    void Update(float deltaTime);
    void SetState(PLAYER_STATE newState);
    PLAYER_STATE GetCurrentState() const { return _stateMachine.GetCurrentState(); }

    void SetPosition(const Vec3& position);
    void SetRotation(const Vec3& rotation);

    uint32_t GetPlayerId() const { return _playerId; }
    bool IsLocal() const { return _isLocal; }

    std::shared_ptr<GameObject> GetGameObject() const { return _gameObject; }
    void PlayAnimation(const PLAYER_STATE state);
    void ApplyGravity(float deltaTime);

private:
    uint32_t _playerId;
    bool _isLocal;
    PlayerStateMachine _stateMachine;
    std::shared_ptr<GameObject> _gameObject;
    std::shared_ptr<Animator> _animator;

    struct PlayerPhysicsState
    {
        physx::PxController* controller;
        physx::PxVec3 moveDirection;
    };

    PlayerPhysicsState _playerPhysics;

public:
    const PlayerPhysicsState& GetPlayerPhysics() { return _playerPhysics; }
};