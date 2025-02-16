#pragma once
#include "GameObject.h"
#include "CharacterController.h"
#include "PlayerStateMachine.h"
#include "CharacterMovement.h"
#include "Animator.h"
#include "EventTypes.h"
#include "Engine.h"

class Player {
public:
    Player(uint32_t playerId, std::shared_ptr<GameObject> gameObject);
    ~Player();

    void Update(float deltaTime);

    // 상태 관리
    PLAYER_STATE GetCurrentState() const;
	bool GetInvincible() const { return m_isInvincible; }
    void SetState(PLAYER_STATE newState);
	void SetInvincible(bool invincible) { m_isInvincible = invincible; }

    // 컴포넌트 접근
    std::shared_ptr<GameObject> GetGameObject() const { return m_gameObject; }
    std::shared_ptr<CharacterController> GetCharacterController() const;
    std::shared_ptr<CharacterMovement> GetMovementComponent() const;
    std::shared_ptr<Animator> GetAnimator() const;

    // 플레이어 정보
    uint32_t GetPlayerId() const { return m_playerId; }
    void PlayAnimation(PLAYER_STATE state);

    void ProcessNetworkInput(const NetworkInputData& inputData);
    bool IsLocalPlayer() const { return m_playerId == GEngine->GetMyPlayerId(); }

private:
    void OnHit(const Event::PlayerHitEvent& event);

private:
    void CreateComponents();
    void InitializeStateMachine();
    void RegisterEventHandlers();

private:
    uint32_t m_playerId;
    size_t m_hitEventId;
    bool m_isInvincible = false;
	float m_maxHealth = 500.0f;
	float m_currentHealth = 500.0f;

    std::shared_ptr<GameObject> m_gameObject;
    PlayerStateMachine m_stateMachine;
};