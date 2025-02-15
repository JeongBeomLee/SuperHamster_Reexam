#include "pch.h"
#include "KeyObject.h"
#include "PhysicsBody.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Timer.h"
#include "SoundSystem.h"
#include "PlayerManager.h"
#include "Resources.h"

int KeyObject::s_keyId = 0;
const std::vector<Vec3> KeyObject::KEY_POSITIONS = {
    Vec3(1947.3978f, 210.00014f, -3446.4392f),
    Vec3(-1945.4989f, 290.41608f, -1794.3052f),
    Vec3(-2439.8887f, 159.999798f, 1845.0043f),
    Vec3(-1544.1814f, 159.999863f, 1856.0829f)
};
KeyObject::KeyObject()
{
	m_keyId = s_keyId++;
}

KeyObject::~KeyObject()
{
	GET_SINGLE(EventManager)->Unsubscribe<Event::TriggerEvent>(m_triggerEventId);
}

void KeyObject::Awake()
{
    CreateKeyObject();

    m_triggerEventId = GET_SINGLE(EventManager)->Subscribe<Event::TriggerEvent>(
        Event::EventCallback<Event::TriggerEvent>(
            [this](const Event::TriggerEvent& event) {
				if (event.triggerEnter) {
					OnTriggerEnter(event);
				}
			})
    );
}

void KeyObject::Start()
{
}

void KeyObject::Update()
{
    if (!GetGameObject()->IsActive()) return;
    if (m_isObtained) {
        UpdateObtainAnimation();
    }
    else {
        UpdateFloatingAnimation();
    }
}

void KeyObject::OnTriggerEnter(const Event::TriggerEvent& event)
{
    GameObject* triggerObject = static_cast<GameObject*>(event.triggerActor->userData);
	if (triggerObject != GetGameObject().get()) return;
    if (!m_isObtained) {
        // 플레이어와의 충돌 확인
        GameObject* otherObject = static_cast<GameObject*>(event.otherActor->userData);
        const auto& players = GET_SINGLE(PlayerManager)->GetPlayers();
		for (const auto& [playerId, player] : players) {
			if (player->GetGameObject().get() == otherObject) {
				// 획득 사운드 재생
				auto sound = GET_SINGLE(Resources)->Get<Sound>(L"KeyObtain");
				if (sound) {
					GET_SINGLE(SoundSystem)->Play3D(sound, GetTransform()->GetLocalPosition());
				}
				m_isObtained = true;
				Logger::Instance().Debug("열쇠 획득됨. 위치: ({}, {}, {})",
					m_initialPosition.x, m_initialPosition.y, m_initialPosition.z);
			}
		}
    }
}

void KeyObject::CreateKeyObject()
{
    m_keyObject = make_shared<GameObject>();
    m_keyObject->SetName(L"KeyModel" + m_keyId);
    m_keyObject->AddComponent(make_shared<Transform>());
    m_initialPosition = GetTransform()->GetLocalPosition();
    m_keyObject->GetTransform()->SetLocalPosition(m_initialPosition);

    // PhysicsBody 설정
    auto physicsBody = make_shared<PhysicsBody>();
    m_keyObject->AddComponent(physicsBody);

    PhysicsBody::BoxParams params;
    params.dimensions = PxVec3(50.0f, 50.0f, 50.0f);

    physicsBody->SetCollisionGroup(CollisionGroup::Trigger);
    physicsBody->SetCollisionMask(CollisionGroup::Player);

    physicsBody->CreateBody(
        PhysicsObjectType::STATIC,
        PhysicsShapeType::Box,
        params
    );

	string name = "keyTrigger" + to_string(m_keyId);
    const char* actorName = name.c_str();
    physicsBody->GetPhysicsObject()->GetActor()->setName(actorName);
    physicsBody->GetPhysicsObject()->GetActor()->userData = GetGameObject().get();

    // 트리거로 설정
    PxShape* shape;
    physicsBody->GetPhysicsObject()->GetActor()->getShapes(&shape, 1);
    shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
    shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

    GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(m_keyObject);
}

void KeyObject::UpdateFloatingAnimation()
{
    m_elapsedTime += DELTA_TIME;

    auto transform = GetTransform();

    // Y축 기준 회전
    Vec3 rotation = transform->GetLocalRotation();
    rotation.y += m_rotationSpeed * DELTA_TIME;
    transform->SetLocalRotation(rotation);

    // 위아래로 부유하는 움직임
    float floatOffset = sin(m_elapsedTime * m_floatSpeed) * m_floatHeight;
    Vec3 position = m_initialPosition;
    position.y += floatOffset;
    transform->SetLocalPosition(position);
}

void KeyObject::UpdateObtainAnimation()
{
    m_obtainTimer += DELTA_TIME;
    float progress = m_obtainTimer / OBTAIN_DURATION;

    if (progress >= 1.0f) {
        GetTransform()->SetLocalPosition(Vec3(0.f, -9999.f, 0.f));
        GetGameObject()->SetActive(false);
        return;
    }

    // 위로 상승하며 크기가 작아지는 애니메이션
    auto transform = GetTransform();
    Vec3 position = m_initialPosition;
    position.y += progress * 100.0f;  // 200 유닛 위로 상승
    transform->SetLocalPosition(position);

    // 크기 감소
    Vec3 scale = m_initialScale * (1.0f - progress);
    transform->SetLocalScale(scale);
}
