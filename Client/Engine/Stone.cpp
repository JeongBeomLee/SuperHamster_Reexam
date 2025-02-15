#include "pch.h"
#include "Stone.h"
#include "EventManager.h"
#include "PlayerManager.h"

Stone::Stone()
{
}

Stone::~Stone()
{
	GET_SINGLE(EventManager)->Unsubscribe<Event::CollisionEvent>(m_collisionEventId);
}

void Stone::Awake()
{
    CreateComponents();
    InitializeAreas();

    // 충돌 이벤트 구독
    m_collisionEventId = GET_SINGLE(EventManager)->Subscribe<Event::CollisionEvent>(
        Event::EventCallback<Event::CollisionEvent>(
            [this](const Event::CollisionEvent& event) {
                OnCollision(event);
            })
    );
}

void Stone::Update()
{
    if (!GetGameObject()->IsActive()) return;

    Vec3 currentPos = GetGameObject()->GetTransform()->GetLocalPosition();
    Vec3 targetPos = m_areas[m_currentAreaIndex];

    // 현재 목표 지점까지의 방향과 거리 계산
    Vec3 direction = targetPos - currentPos;
    direction.y = 0.0f;
    float distance = direction.Length();

    // 목표 지점에 도달하면 다음 영역으로 이동
    if (distance < 50.0f) {
        MoveToNextArea();
        return;
    }

    // 이동 방향 정규화 및 이동
    direction.Normalize();
    m_movement->SetMoveDirection(direction);
}

void Stone::OnCollision(const Event::CollisionEvent& event)
{
    GameObject* otherObject = static_cast<GameObject*>(event.actor2->userData);
    if (!otherObject) return;

    const auto& players = GET_SINGLE(PlayerManager)->GetPlayers();
    for (const auto& [playerId, player] : players) {
        if(player->GetGameObject().get() == otherObject && 
            IsPlayerInRange(otherObject->GetTransform()->GetWorldPosition())) {
			DealDamageToPlayer(player.get());
        }
    }
}

void Stone::MoveToNextArea()
{
    m_currentAreaIndex = (m_currentAreaIndex + 1) % m_areas.size();
}

void Stone::InitializeAreas()
{
    m_areas.push_back(Vec3(-1600.0f, 250.0f, -1500.0f)); // Area 1
    m_areas.push_back(Vec3(-1600.0f, 250.0f, -2100.0f)); // Area 2 
    m_areas.push_back(Vec3(-2300.0f, 250.0f, -2100.0f)); // Area 3
    m_areas.push_back(Vec3(-2300.0f, 250.0f, -1500.0f)); // Area 4
}

void Stone::CreateComponents()
{
    m_controller = make_shared<CharacterController>();
    GetGameObject()->AddComponent(m_controller);

    m_controller->SetCollisionGroup(CollisionGroup::Obstacle);
    m_controller->SetCollisionMask(
        CollisionGroup::Default |
        CollisionGroup::Ground |
        CollisionGroup::Player
    );
    m_controller->SetRadius(100.f);
    m_controller->SetHeight(200.f);
    m_controller->SetIsGravityEnabled(false);
    m_controller->Initialize();

    // Movement 컴포넌트 추가
    m_movement = make_shared<CharacterMovement>();
    m_movement->SetMoveSpeed(MOVE_SPEED);
    GetGameObject()->AddComponent(m_movement);
}

bool Stone::IsPlayerInRange(const Vec3& playerPos)
{
    Vec3 myPos = GetTransform()->GetLocalPosition();
    return (playerPos - myPos).Length() <= ATTACK_RANGE;
}

void Stone::DealDamageToPlayer(Player* player)
{
    Event::PlayerHitEvent event(
        player,
        GetGameObject().get(),
        ATTACK_DAMAGE,
        player->GetGameObject()->GetTransform()->GetWorldPosition()
    );

	Logger::Instance().Debug("Stone이 플레이어에게 데미지 {}를 입힘", ATTACK_DAMAGE);
    GET_SINGLE(EventManager)->Publish(event);
}
