#pragma once
#include "MonoBehaviour.h"
#include "CharacterController.h"
#include "CharacterMovement.h"
#include "EventTypes.h"

class Stone : public MonoBehaviour 
{
public:
    Stone();
    virtual ~Stone();

    void Awake() override;
    void Update() override;
    void OnCollision(const Event::CollisionEvent& event);

private:
    void MoveToNextArea();
    void InitializeAreas();
    void CreateComponents();
    bool IsPlayerInRange(const Vec3& playerPos);
    void DealDamageToPlayer(Player* player);

private:
    static constexpr float MOVE_SPEED = 500.0f;
    static constexpr float ATTACK_RANGE = 200.0f;
    static constexpr float ATTACK_DAMAGE = 10.0f;

    std::vector<Vec3> m_areas;          // �̵� ���� ��ġ��
    int m_currentAreaIndex = 0;         // ���� ���� �ε���
    size_t m_collisionEventId;          // �浹 �̺�Ʈ ID

    std::shared_ptr<CharacterController> m_controller;
    std::shared_ptr<CharacterMovement> m_movement;
};