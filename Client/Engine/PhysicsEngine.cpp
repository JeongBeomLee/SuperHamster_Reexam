#include "pch.h"
#include "PhysicsEngine.h"
#include "Timer.h"
#include "Logger.h"

PhysicsEngine::PhysicsEngine()
	: m_foundation(nullptr)
	, m_physics(nullptr)
	, m_dispatcher(nullptr)
	, m_scene(nullptr)
	, m_defaultMaterial(nullptr)
	, m_pvd(nullptr)
	, m_controllerManager(nullptr)
	, m_obstacleContext(nullptr)
{
}

PhysicsEngine::~PhysicsEngine()
{
	Cleanup();
}

bool PhysicsEngine::Initialize()
{
	if (!CreateFoundation()) return false;
	SetupDebugger();
	if (!CreatePhysics()) return false;
	if (!CreateScene()) return false;
	if (!CreateDefaultMaterial()) return false;
	if (!CreateControllerManager()) return false;
	if (!CreateObstacleContext()) return false;

	return true;
}

void PhysicsEngine::Update()
{
	float deltaTime = DELTA_TIME;
	if (m_scene) {
		try {
			// ��ֹ� ������Ʈ
			UpdateObstacles();

			// simulate ȣ�� �� ���� üũ
			m_scene->simulate(deltaTime);

			// simulate�� ����� ��ٸ�
			m_scene->fetchResults(true);
		}
		catch (const std::exception& e) {
			Logger::Instance().Error("PhysicsEngine::Update - Exception: {}", e.what());
		}
	}
	else {
		Logger::Instance().Error("PhysicsEngine::Update - Scene is null");
	}
}

void PhysicsEngine::UpdateObstacles()
{
	if (!m_obstacleContext) return;

	// ��ֹ� ������Ʈ
	for (const auto& [handle, obstacle] : m_obstacles) {
		m_obstacleContext->updateObstacle(handle, *obstacle);
	}
}

void PhysicsEngine::Cleanup()
{
	// ��ֹ� ����
	ClearObstacles();

	// ��ֹ� ���ؽ�Ʈ ����
	PX_RELEASE(m_obstacleContext);

	m_contactCallback.reset();
	m_controllerHitReport.reset();

	// ��Ʈ�ѷ� �Ŵ��� ����
	m_controllerManager->purgeControllers();
	PX_RELEASE(m_controllerManager);

	PX_RELEASE(m_scene);
	PX_RELEASE(m_dispatcher);
	PX_RELEASE(m_physics);
	if (m_pvd) {
		PxPvdTransport* transport = m_pvd->getTransport();
		PX_RELEASE(m_pvd);
		PX_RELEASE(transport);
	}
	PX_RELEASE(m_foundation);
}

std::shared_ptr<PhysicsObject> PhysicsEngine::CreateBox(
	const PxVec3& position,
	const PxVec3& dimensions,
	PhysicsObjectType type,
	CollisionGroup group, CollisionGroup mask,
	float density)
{
	PxRigidActor* actor = nullptr;
	PxShape* shape = nullptr;

	switch (type) {
	case PhysicsObjectType::STATIC: {
		actor = m_physics->createRigidStatic(PxTransform(position));
		actor->setName("StaticBox");
		shape = PxRigidActorExt::createExclusiveShape(*actor,
			PxBoxGeometry(dimensions), *m_defaultMaterial);
		break;
	}
	case PhysicsObjectType::DYNAMIC: {
		PxRigidDynamic* dynamicActor = m_physics->createRigidDynamic(PxTransform(position));
		dynamicActor->setName("DynamicBox");
		shape = PxRigidActorExt::createExclusiveShape(*dynamicActor,
			PxBoxGeometry(dimensions), *m_defaultMaterial);
		PxRigidBodyExt::updateMassAndInertia(*dynamicActor, density);
		actor = dynamicActor;
		break;
	}
	default:
		break;
	}

	if (actor && shape) {
		// �浹 ���� ������ ����
		shape->setSimulationFilterData(CreateFilterData(group, mask));
		shape->setQueryFilterData(CreateFilterData(group, mask));
		m_scene->addActor(*actor);
		auto physicsObject = std::make_shared<PhysicsObject>(actor);
		m_physicsObjects.push_back(physicsObject);
		return physicsObject;
	}

	return nullptr;
}

std::shared_ptr<PhysicsObject> PhysicsEngine::CreateSphere(
	const PxVec3& position, float radius, PhysicsObjectType type,
	CollisionGroup group, CollisionGroup mask, float density) {

	PxRigidActor* actor = nullptr;
	PxShape* shape = nullptr;

	switch (type) {
	case PhysicsObjectType::STATIC: {
		actor = m_physics->createRigidStatic(PxTransform(position));
		actor->setName("StaticSphere");
		shape = PxRigidActorExt::createExclusiveShape(*actor,
			PxSphereGeometry(radius), *m_defaultMaterial);
		break;
	}
	case PhysicsObjectType::DYNAMIC: {
		PxRigidDynamic* dynamicActor = m_physics->createRigidDynamic(PxTransform(position));
		dynamicActor->setName("DynamicSphere");
		shape = PxRigidActorExt::createExclusiveShape(*dynamicActor,
			PxSphereGeometry(radius), *m_defaultMaterial);
		PxRigidBodyExt::updateMassAndInertia(*dynamicActor, density);
		actor = dynamicActor;
		break;
	}
	}

	if (actor && shape) {
		shape->setSimulationFilterData(CreateFilterData(group, mask));
		shape->setQueryFilterData(CreateFilterData(group, mask));
		m_scene->addActor(*actor);
		auto physicsObject = std::make_shared<PhysicsObject>(actor);
		m_physicsObjects.push_back(physicsObject);
		return physicsObject;
	}

	return nullptr;
}

std::shared_ptr<PhysicsObject> PhysicsEngine::CreateCapsule(
	const PxVec3& position, float radius, float halfHeight, PhysicsObjectType type,
	CollisionGroup group, CollisionGroup mask, float density) {

	PxRigidActor* actor = nullptr;
	PxShape* shape = nullptr;

	switch (type) {
	case PhysicsObjectType::STATIC: {
		actor = m_physics->createRigidStatic(
			PxTransform(position));
		actor->setName("StaticCapsule");
		shape = PxRigidActorExt::createExclusiveShape(*actor,
			PxCapsuleGeometry(radius, halfHeight), *m_defaultMaterial);
		break;
	}
	case PhysicsObjectType::DYNAMIC: {
		PxRigidDynamic* dynamicActor = m_physics->createRigidDynamic(
			PxTransform(position));
		dynamicActor->setName("DynamicCapsule");
		shape = PxRigidActorExt::createExclusiveShape(*dynamicActor,
			PxCapsuleGeometry(radius, halfHeight), *m_defaultMaterial);
		PxRigidBodyExt::updateMassAndInertia(*dynamicActor, density);
		actor = dynamicActor;
		break;
	}
	}

	if (actor && shape) {
		shape->setSimulationFilterData(CreateFilterData(group, mask));
		shape->setQueryFilterData(CreateFilterData(group, mask));
		m_scene->addActor(*actor);
		auto physicsObject = std::make_shared<PhysicsObject>(actor);
		m_physicsObjects.push_back(physicsObject);
		return physicsObject;
	}

	return nullptr;
}

std::shared_ptr<PhysicsObject> PhysicsEngine::CreateTriangleMesh(
	const PxVec3& position,
	const std::vector<PxVec3>& vertices,
	const std::vector<uint32_t>& indices,
	PhysicsObjectType type,
	CollisionGroup group,
	CollisionGroup mask) {

	// �޽� ������ ����
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = static_cast<PxU32>(vertices.size());
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = vertices.data();
	meshDesc.triangles.count = static_cast<PxU32>(indices.size() / 3);
	meshDesc.triangles.stride = 3 * sizeof(uint32_t);
	meshDesc.triangles.data = indices.data();

	PxTolerancesScale scale;
	PxCookingParams params(scale);

	// �޽� ��ŷ
	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);
	if (!status) return nullptr;

	// ��ŷ�� �����ͷ� �޽� ����
	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxTriangleMesh* triangleMesh = m_physics->createTriangleMesh(readBuffer);
	if (!triangleMesh) return nullptr;

	PxRigidActor* actor = nullptr;
	PxShape* shape = nullptr;

	// �޽ô� �ַ� ���� ��ü�� ���
	// x�� ���� -90�� ȸ��, y�� ���� 90�� ȸ��
	physx::PxQuat rotX = physx::PxQuat(-PxHalfPi, PxVec3(1, 0, 0));
	physx::PxQuat rotY = physx::PxQuat(PxHalfPi, PxVec3(0, 1, 0));
	physx::PxQuat rotation = rotY * rotX;

	// �޽� ȸ�� �� ����
	physx::PxTransform pose = physx::PxTransform(position, rotation);
	actor = m_physics->createRigidStatic(pose);

	//actor = m_physics->createRigidStatic(PxTransform(position, rotX));
	//actor->setName("TriangleMesh");

	shape = PxRigidActorExt::createExclusiveShape(*actor,
		PxTriangleMeshGeometry(triangleMesh), *m_defaultMaterial);

	if (actor && shape) {
		shape->setSimulationFilterData(CreateFilterData(group, mask));
		shape->setQueryFilterData(CreateFilterData(group, mask));
		m_scene->addActor(*actor);
		auto physicsObject = std::make_shared<PhysicsObject>(actor);
		m_physicsObjects.push_back(physicsObject);
		return physicsObject;
	}

	return nullptr;
}

std::shared_ptr<PhysicsObject> PhysicsEngine::CreateGroundPlane()
{
    PxRigidStatic* groundPlane = PxCreatePlane(*m_physics, PxPlane(0, 1, 0, 0), *m_defaultMaterial);
    PxShape* shapes[1];
    groundPlane->getShapes(shapes, 1);
	auto group = CollisionGroup::Ground;
	auto mask = 
		CollisionGroup::Default | 
		CollisionGroup::Character | 
		CollisionGroup::Enemy | 
		CollisionGroup::Projectile | 
		CollisionGroup::Trigger | 
		CollisionGroup::Obstacle;
    if (groundPlane) {
        // �浹 ���� ������ ����
		shapes[0]->setSimulationFilterData(CreateFilterData(group, mask));
		shapes[0]->setQueryFilterData(CreateFilterData(group, mask));
        m_scene->addActor(*groundPlane);
        auto physicsObject = std::make_shared<PhysicsObject>(groundPlane);
        m_physicsObjects.push_back(physicsObject);
        return physicsObject;
    }
    return nullptr;
}

std::shared_ptr<PhysicsObject> PhysicsEngine::CreateCapsuleController(
	const PxVec3& position, float radius, float height,
	CollisionGroup group, CollisionGroup mask)
{
	if (!m_controllerManager) {
		Logger::Instance().Error("��Ʈ�ѷ� �Ŵ����� �ʱ�ȭ���� ����");
		return nullptr;
	}

	// ĸ�� ��Ʈ�ѷ� �Ӽ� ����
	PxCapsuleControllerDesc desc;
	desc.position = PxExtendedVec3(position.x, position.y, position.z);
	desc.radius = radius;
	desc.height = height;
	desc.stepOffset = 0.1f;          // ��� ��� ����
	desc.slopeLimit = cosf(physx::PxDegToRad(45.f)); // �÷��̾� �̵� �� �ö� �� �ִ� ��� ����
	desc.material = m_defaultMaterial;
	desc.upDirection = PxVec3(0, 1, 0);
	desc.reportCallback = m_controllerHitReport.get();    // �浹 �ݹ�
	desc.behaviorCallback = nullptr;  // �ൿ �ݹ�
	desc.contactOffset = 0.1f;        // ���� ���� �Ÿ�

	// ��Ʈ�ѷ� ����
	PxController* controller = m_controllerManager->createController(desc);
	if (!controller) {
		Logger::Instance().Error("ĸ�� ��Ʈ�ѷ� ���� ����");
		return nullptr;
	}

	// ���� ������ ����
	PxRigidDynamic* actor = controller->getActor();
	PxShape* shape;
	actor->getShapes(&shape, 1);

	// �ùķ��̼� ���� ������ ����
	shape->setSimulationFilterData(CreateFilterData(group, mask));

	// ���� ���� �����͵� �����ϰ� ���� (����ĳ��Ʈ � ���)
	shape->setQueryFilterData(CreateFilterData(group, mask));

	// ����� ���ͷ� ����
	actor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	// PhysicsObject ���� �� ��ȯ
	auto physicsObject = std::make_shared<PhysicsObject>(actor);
	m_physicsObjects.push_back(physicsObject);

	Logger::Instance().Debug("ĸ�� ��Ʈ�ѷ� ������. ��ġ: ({}, {}, {}), ������: {}, ����: {}",
		position.x, position.y, position.z, radius, height);

	return physicsObject;
}


PxObstacleHandle PhysicsEngine::AddObstacle(
	const PxVec3& position, const PxVec3& dimensions, const PxQuat& rotation)
{
	if (!m_obstacleContext) {
		Logger::Instance().Error("��ֹ� ���ؽ�Ʈ�� �ʱ�ȭ���� ����");
		return PxObstacleHandle(0);
	}

	auto obstacle = std::make_unique<PxBoxObstacle>();
	obstacle->mPos = PxExtendedVec3(position.x, position.y, position.z);
	obstacle->mHalfExtents = dimensions * 0.5f;
	obstacle->mRot = PxQuat(rotation);

	PxObstacleHandle handle = m_obstacleContext->addObstacle(*obstacle);
	if (handle != PxObstacleHandle(0)) {
		m_obstacles.emplace_back(handle, std::move(obstacle));

		Logger::Instance().Debug("��ֹ� �߰���. �ڵ�: {}, ��ġ: ({}, {}, {}), ũ��: ({}, {}, {})",
			handle,
			position.x, position.y, position.z,
			dimensions.x, dimensions.y, dimensions.z);
	}

	return handle;
}

void PhysicsEngine::RemoveObstacle(PxObstacleHandle handle)
{
	if (!m_obstacleContext) return;

	auto it = std::find_if(m_obstacles.begin(), m_obstacles.end(),
		[handle](const auto& pair) { return pair.first == handle; });

	if (it != m_obstacles.end()) {
		m_obstacleContext->removeObstacle(handle);
		m_obstacles.erase(it);
		Logger::Instance().Debug("��ֹ� ���ŵ�. �ڵ�: {}", handle);
	}
}

void PhysicsEngine::ClearObstacles()
{
	if (!m_obstacleContext) return;

	for (const auto& [handle, obstacle] : m_obstacles) {
		m_obstacleContext->removeObstacle(handle);
	}
	m_obstacles.clear();
	Logger::Instance().Debug("��� ��ֹ� ���ŵ�");
}

PxFilterData PhysicsEngine::CreateFilterData(CollisionGroup group, CollisionGroup mask)
{
	PxFilterData filterData;
	filterData.word0 = static_cast<PxU32>(group);  // �ڽ��� �׷�
	filterData.word1 = static_cast<PxU32>(mask);   // �浹�� �׷�
	return filterData;
}

bool PhysicsEngine::CreateFoundation()
{
	m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_errorCallback);
	return m_foundation != nullptr;
}

bool PhysicsEngine::CreatePhysics()
{
	PxTolerancesScale scale;
	m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, scale, true, m_pvd);
	return m_physics != nullptr;
}

bool PhysicsEngine::CreateScene()
{
	PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	// CPU ����ó ���� (��Ƽ������)
	m_dispatcher = PxDefaultCpuDispatcherCreate(2);  // 2���� ��Ŀ ������
	if (!m_dispatcher) return false;
	sceneDesc.cpuDispatcher = m_dispatcher;

	// �浹 �ݹ� ���� �� ����
	m_contactCallback = std::make_unique<ContactReportCallback>();
	sceneDesc.simulationEventCallback = m_contactCallback.get();

	// Ŀ���� ���� ���̴� ����
	sceneDesc.filterShader = CustomFilterShader;

	m_scene = m_physics->createScene(sceneDesc);
	return m_scene != nullptr;
}

bool PhysicsEngine::CreateDefaultMaterial()
{
	// �⺻ ���� material ����
	m_defaultMaterial = m_physics->createMaterial(0.5f, 0.5f, 0.4f); // ���� ����, ���� ����, �ݹ߷�
	if (!m_defaultMaterial) return false;

	return true;
}

bool PhysicsEngine::CreateControllerManager()
{
	// ��Ʈ�ѷ� �Ŵ��� ����
	m_controllerHitReport = std::make_unique<CharacterControllerHitReport>();
	m_controllerManager = PxCreateControllerManager(*m_scene);
	if (!m_controllerManager) {
		Logger::Instance().Error("��Ʈ�ѷ� �Ŵ��� ���� ����");
		return false;
	}

	return true;
}

bool PhysicsEngine::CreateObstacleContext()
{
	// ��ֹ� ���ؽ�Ʈ ����
	m_obstacleContext = m_controllerManager->createObstacleContext();
	if (!m_obstacleContext) {
		Logger::Instance().Error("��ֹ� ���ؽ�Ʈ ���� ����");
		return false;
	}
}

void PhysicsEngine::SetupDebugger()
{
	// PhysX Visual Debugger ����
	m_pvd = PxCreatePvd(*m_foundation);
	if (m_pvd) {
		PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		m_pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
	}
}

PxFilterFlags PhysicsEngine::CustomFilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	// Ʈ���� ó��
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1)) {
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	// �浹 ����ũ Ȯ��
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1)) {
		// CCT ���͸� - CCT�� ����� ���ͷ� ��޵�
		bool isKinematic0 = PxFilterObjectIsKinematic(attributes0);
		bool isKinematic1 = PxFilterObjectIsKinematic(attributes1);

		// CCT�� ���õ� �浹�� ���
		if (isKinematic0 || isKinematic1) {
			pairFlags = PxPairFlag::eMODIFY_CONTACTS
				| PxPairFlag::eDETECT_CCD_CONTACT  // CCD Ȱ��ȭ
				| PxPairFlag::eNOTIFY_TOUCH_CCD    // CCD ���� �˸�
				| PxPairFlag::eNOTIFY_TOUCH_FOUND  // ���� ���� �˸�
				| PxPairFlag::eNOTIFY_TOUCH_LOST   // ���� ���� �˸�
				| PxPairFlag::eNOTIFY_CONTACT_POINTS; // ������ �˸�
		}
		else {
			pairFlags = PxPairFlag::eCONTACT_DEFAULT
				| PxPairFlag::eNOTIFY_TOUCH_FOUND
				| PxPairFlag::eNOTIFY_TOUCH_LOST
				| PxPairFlag::eNOTIFY_CONTACT_POINTS;
		}

		return PxFilterFlag::eDEFAULT;
	}

	return PxFilterFlag::eSUPPRESS;
}
