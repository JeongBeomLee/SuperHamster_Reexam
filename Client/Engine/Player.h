#pragma once
#include "GameObject.h"
#include "CharacterController.h"
#include "PlayerStateMachine.h"
#include "PlayerMovement.h"
#include "Animator.h"

class Player {
public:
    Player(uint32_t playerId, bool isLocal, std::shared_ptr<GameObject> gameObject);
    ~Player();

    //void Initialize();
    void Update(float deltaTime);

    // ���� ����
    void SetState(PLAYER_STATE newState);
    PLAYER_STATE GetCurrentState() const;

    // ������Ʈ ����
    std::shared_ptr<GameObject> GetGameObject() const { return m_gameObject; }
    std::shared_ptr<CharacterController> GetCharacterController() const;
    std::shared_ptr<PlayerMovement> GetMovementComponent() const;
    std::shared_ptr<Animator> GetAnimator() const;

    // �÷��̾� ����
    uint32_t GetPlayerId() const { return m_playerId; }
    bool IsLocal() const { return m_isLocal; }

    void PlayAnimation(PLAYER_STATE state);

private:
    void CreateComponents();
    void InitializeStateMachine();

private:
    uint32_t m_playerId;
    bool m_isLocal;

    std::shared_ptr<GameObject> m_gameObject;
    PlayerStateMachine m_stateMachine;
};