#include "pch.h"
#include "TeleportSystem.h"
#include "Engine.h"
#include "PhysicsEngine.h"
#include "CharacterController.h"
#include "Transform.h"
#include "Resources.h"

#include "MeshRenderer.h"
#include "PhysicsBody.h"
#include "Scene.h"
#include "SceneManager.h"

void TeleportSystem::Initialize()
{
    m_teleportZones[TeleportZoneId::STAGE1_TO_STAGE2] = TeleportZone(
        TeleportZoneId::STAGE1_TO_STAGE2,
        Vec3(-7.1202483f, 125.2015f, -550.79706f),     // �ҽ� ��ġ
        Vec3(-7.1733503f, 124.24036f, -1230.4467f),    // ������ ��ġ
        Vec3(100.0f, 100.0f, 50.0f),                     // Ʈ���� ũ��
        "Stage1", "Stage2"
    );

	m_teleportZones[TeleportZoneId::STAGE2_TO_STAGE1] = TeleportZone(
		TeleportZoneId::STAGE2_TO_STAGE1,
		Vec3(-7.1733503f, 124.24036f, -1230.4467f),    // �ҽ� ��ġ
		Vec3(-7.1202483f, 125.2015f, -550.79706f),     // ������ ��ġ
		Vec3(100.0f, 100.0f, 50.0f),                     // Ʈ���� ũ��
		"Stage2", "Stage1"
	);

	m_teleportZones[TeleportZoneId::STAGE2_TO_STAGE3] = TeleportZone(
		TeleportZoneId::STAGE2_TO_STAGE3,
		Vec3(615.2723f, 126.45162f, -1786.1421f),    // �ҽ� ��ġ
		Vec3(1332.5443f, 126.9432f, -1799.5829f),    // ������ ��ġ
		Vec3(50.0f, 100.0f, 100.0f),                     // Ʈ���� ũ��
		"Stage2", "Stage3"
	);

	m_teleportZones[TeleportZoneId::STAGE2_TO_STAGE9] = TeleportZone(
		TeleportZoneId::STAGE2_TO_STAGE9,
		Vec3(-3.5159104f, 125.41988f, -2292.673f),    // �ҽ� ��ġ
		Vec3(-5.91688f, 126.898865f, -2945.7715f),    // ������ ��ġ
		Vec3(100.0f, 100.0f, 50.0f),                     // Ʈ���� ũ��
		"Stage2", "Stage9"
	);

	m_teleportZones[TeleportZoneId::STAGE2_TO_STAGE6] = TeleportZone(
		TeleportZoneId::STAGE2_TO_STAGE6,
		Vec3(-630.32385f, 120.00408f, -1786.1421f),    // �ҽ� ��ġ
		Vec3(-1336.0951f, 76.06581f, -1793.0691f),    // ������ ��ġ
		Vec3(50.0f, 100.0f, 100.0f),                     // Ʈ���� ũ��
		"Stage2", "Stage6"
	);

	m_teleportZones[TeleportZoneId::STAGE3_TO_STAGE2] = TeleportZone(
		TeleportZoneId::STAGE3_TO_STAGE2,
		Vec3(1332.5443f, 126.9432f, -1799.5829f),    // �ҽ� ��ġ
		Vec3(615.2723f, 126.45162f, -1786.1421f),    // ������ ��ġ
		Vec3(50.0f, 100.0f, 100.0f),                     // Ʈ���� ũ��
		"Stage3", "Stage2"
	);

	m_teleportZones[TeleportZoneId::STAGE3_TO_STAGE4] = TeleportZone(
		TeleportZoneId::STAGE3_TO_STAGE4,
		Vec3(1953.7719f, 124.85406f, -1180.7566f),    // �ҽ� ��ġ
		Vec3(1932.7501f, 125.41099f, -579.214f),    // ������ ��ġ
		Vec3(100.0f, 100.0f, 50.0f),                     // Ʈ���� ũ��
		"Stage3", "Stage4"
	);

	m_teleportZones[TeleportZoneId::STAGE3_TO_STAGE5] = TeleportZone(
		TeleportZoneId::STAGE3_TO_STAGE5,
		Vec3(1953.7719f, 125.86808f, -2300.4028f),    // �ҽ� ��ġ
		Vec3(1946.4819f, 125.04549f, -2901.788f),    // ������ ��ġ
		Vec3(100.0f, 100.0f, 50.0f),                     // Ʈ���� ũ��
		"Stage3", "Stage5"
	);

	m_teleportZones[TeleportZoneId::STAGE4_TO_STAGE3] = TeleportZone(
		TeleportZoneId::STAGE4_TO_STAGE3,
		Vec3(1932.7501f, 125.41099f, -579.214f),    // �ҽ� ��ġ
		Vec3(1953.7719f, 124.85406f, -1180.7566f),    // ������ ��ġ
		Vec3(100.0f, 100.0f, 50.0f),                     // Ʈ���� ũ��
		"Stage4", "Stage3"
	);

	m_teleportZones[TeleportZoneId::STAGE5_TO_STAGE3] = TeleportZone(
		TeleportZoneId::STAGE5_TO_STAGE3,
		Vec3(1946.4819f, 125.04549f, -2901.788f),    // �ҽ� ��ġ
		Vec3(1953.7719f, 125.86808f, -2300.4028f),    // ������ ��ġ
		Vec3(100.0f, 100.0f, 50.0f),                     // Ʈ���� ũ��
		"Stage5", "Stage3"
	);

	m_teleportZones[TeleportZoneId::STAGE6_TO_STAGE2] = TeleportZone(
		TeleportZoneId::STAGE6_TO_STAGE2,
		Vec3(-1336.0951f, 76.06581f, -1793.0691f),    // �ҽ� ��ġ
		Vec3(-630.32385f, 120.00408f, -1786.1421f),    // ������ ��ġ
		Vec3(50.0f, 100.0f, 100.0f),                     // Ʈ���� ũ��
		"Stage5", "Stage6"
	);

	m_teleportZones[TeleportZoneId::STAGE6_TO_STAGE7] = TeleportZone(
		TeleportZoneId::STAGE6_TO_STAGE7,
		Vec3(-2340.9502f, 74.05789f, -1273.6133f),    // �ҽ� ��ġ
		Vec3(-2353.169f, 25.030746f, -655.33057f),    // ������ ��ġ
		Vec3(100.0f, 100.0f, 50.0f),                     // Ʈ���� ũ��
		"Stage6", "Stage7"
	);

	m_teleportZones[TeleportZoneId::STAGE7_TO_STAGE6] = TeleportZone(
		TeleportZoneId::STAGE7_TO_STAGE6,
		Vec3(-2353.169f, 25.030746f, -655.33057f),    // �ҽ� ��ġ
		Vec3(-2340.9502f, 74.05789f, -1273.6133f),    // ������ ��ġ
		Vec3(100.0f, 100.0f, 50.0f),                     // Ʈ���� ũ��
		"Stage7", "Stage6"
	);

	m_teleportZones[TeleportZoneId::STAGE7_TO_STAGE8] = TeleportZone(
		TeleportZoneId::STAGE7_TO_STAGE8,
		Vec3(-2354.2197f, 25.150675f, 448.38303f),    // �ҽ� ��ġ
		Vec3(-2352.1113f, 75.47251f, 954.99695f),    // ������ ��ġ
		Vec3(100.0f, 100.0f, 50.0f),                     // Ʈ���� ũ��
		"Stage7", "Stage8"
	);

	m_teleportZones[TeleportZoneId::STAGE8_TO_STAGE7] = TeleportZone(
		TeleportZoneId::STAGE8_TO_STAGE7,
		Vec3(-2352.1113f, 75.47251f, 954.99695f),    // �ҽ� ��ġ
		Vec3(-2354.2197f, 25.150675f, 448.38303f),    // ������ ��ġ
		Vec3(100.0f, 100.0f, 50.0f),                     // Ʈ���� ũ��
		"Stage8", "Stage7"
	);

	m_teleportZones[TeleportZoneId::STAGE9_TO_STAGE2] = TeleportZone(
		TeleportZoneId::STAGE9_TO_STAGE2,
		Vec3(-5.91688f, 126.898865f, -2945.7715f),    // �ҽ� ��ġ
		Vec3(-3.5159104f, 125.41988f, -2292.673f),    // ������ ��ġ
		Vec3(100.0f, 100.0f, 50.0f),                     // Ʈ���� ũ��
		"Stage8", "Stage9"
	);

	CreateTeleportZones();
	Logger::Instance().Info("�ڷ���Ʈ �ý��� �ʱ�ȭ �Ϸ�");
}

void TeleportSystem::Update()
{
	float deltaTime = DELTA_TIME;

	// ��ٿ� �ð� ������Ʈ
	auto it = m_cooldowns.begin();
	while (it != m_cooldowns.end()) {
		it->second -= deltaTime;
		if (it->second <= 0.0f) {
			it = m_cooldowns.erase(it);
		}
		else {
			++it;
		}
	}
}

void TeleportSystem::CreateTeleportZones()
{
	for (const auto& [id, zone] : m_teleportZones) {
		CreateTeleportTrigger(
			zone.sourcePosition,
			zone.triggerSize,
			id
		);

		/*auto trigger = CreateTeleportTrigger(
			zone.sourcePosition,
			zone.triggerSize,
			id
		);*/
		
		/*if (trigger) {
			m_triggerObjects.push_back(trigger);
		}*/
	}
}

const TeleportZone* TeleportSystem::GetTeleportZone(TeleportZoneId id) const
{
	auto it = m_teleportZones.find(id);
	return it != m_teleportZones.end() ? &it->second : nullptr;
}

void TeleportSystem::TriggerTeleport(GameObject* player, TeleportZoneId id)
{
	// ��ٿ� üũ
	if (IsInCooldown(player)) {
		Logger::Instance().Debug("�ڷ���Ʈ ��ٿ� �߿��� �� �̵� �Ұ�");
		return;
	}

	const TeleportZone* zone = GetTeleportZone(id);
	if (!zone) {
		Logger::Instance().Error("�ڷ���Ʈ ���� ã�� �� ����: {}", static_cast<int>(id));
		return;
	}

	auto controller = player->GetCharacterController();
	if (!controller) {
		Logger::Instance().Error("ĳ���� ��Ʈ�ѷ��� ã�� �� ����");
		return;
	}

	// �ڷ���Ʈ ����
	controller->Teleport(zone->destinationPosition);

	// ��ٿ� ����
	StartCooldown(player);

	Logger::Instance().Info("�ڷ���Ʈ ����: {} -> {}",
		zone->sourceName, zone->destinationName);
}

void TeleportSystem::DebugTeleportZones()
{
	for (const auto& [id, zone] : m_teleportZones) {
		Logger::Instance().Debug("�ڷ���Ʈ �� ����:");
		Logger::Instance().Debug("ID: {}", static_cast<int>(id));
		Logger::Instance().Debug("�����: {} ({}, {}, {})",
			zone.sourceName,
			zone.sourcePosition.x,
			zone.sourcePosition.y,
			zone.sourcePosition.z);
		Logger::Instance().Debug("������: {} ({}, {}, {})",
			zone.destinationName,
			zone.destinationPosition.x,
			zone.destinationPosition.y,
			zone.destinationPosition.z);
	}
}

std::shared_ptr<GameObject> TeleportSystem::CreateTeleportTrigger(const Vec3& position, const Vec3& size, TeleportZoneId id)
{
	auto triggerObject = make_shared<GameObject>();

	auto meshRenderer = make_shared<MeshRenderer>();
	auto transform = make_shared<Transform>();
	auto physicsBody = make_shared<PhysicsBody>();

	triggerObject->AddComponent(meshRenderer);
	triggerObject->AddComponent(transform);
	triggerObject->AddComponent(physicsBody);

	auto mesh = GET_SINGLE(Resources)->LoadCubeMesh(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);
	auto material = GET_SINGLE(Resources)->Get<Material>(L"DebugVisualization")->Clone();
	material->SetVec4(0, Vec4(1.f, 0.f, 0.f, 1.f));
	meshRenderer->SetMesh(mesh);
	meshRenderer->SetMaterial(material);

	auto newPosition = position;
	newPosition.y += size.y;
	transform->SetLocalPosition(newPosition);

	PhysicsBody::BoxParams params;
	params.dimensions = PxVec3(size.x, size.y, size.z);
	physicsBody->SetCollisionGroup(CollisionGroup::Trigger);
	physicsBody->SetCollisionMask(
		CollisionGroup::Default |
		CollisionGroup::Character
	);

	physicsBody->CreateBody(
		PhysicsObjectType::STATIC,
		PhysicsShapeType::Box,
		params
	);

	physicsBody->GetPhysicsObject()->GetActor()->setName("triggerObject");
	physicsBody->GetPhysicsObject()->GetActor()->userData = new TeleportZoneId(id);

	PxShape* shape;
	physicsBody->GetPhysicsObject()->GetActor()->getShapes(&shape, 1);

	// Ʈ���ŷ� ����
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(triggerObject);

	return triggerObject;
}

bool TeleportSystem::IsInCooldown(GameObject* player) const
{
	auto it = m_cooldowns.find(player);
	if (it == m_cooldowns.end())
		return false;

	return it->second > 0.0f;
}

void TeleportSystem::StartCooldown(GameObject* player)
{
	m_cooldowns[player] = TELEPORT_COOLDOWN;
}
