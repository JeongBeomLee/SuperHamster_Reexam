#pragma once
#include "PhysicsTypes.h"
#include "ContactReportCallback.h"
#include "CharacterControllerHitReport.h"
#include "PhysicsObject.h"

class PhysicsEngine {
public:
	PhysicsEngine();
	~PhysicsEngine();

	bool Initialize();
	void Update();
	void UpdateObstacles();
	void Cleanup();

	// 물리 객체 생성 함수들
	std::shared_ptr<PhysicsObject> CreateBox(
		const PxVec3& position,
		const PxVec3& dimensions,
		PhysicsObjectType type = PhysicsObjectType::DYNAMIC,
		CollisionGroup group = CollisionGroup::Default,      // 추가
		CollisionGroup mask = CollisionGroup::Default,       // 추가
		float density = 1.0f);

	std::shared_ptr<PhysicsObject> CreateSphere(
		const PxVec3& position,
		float radius,
		PhysicsObjectType type = PhysicsObjectType::DYNAMIC,
		CollisionGroup group = CollisionGroup::Default,
		CollisionGroup mask = CollisionGroup::Default,
		float density = 1.0f);

	std::shared_ptr<PhysicsObject> CreateCapsule(
		const PxVec3& position,
		float radius,
		float halfHeight,
		PhysicsObjectType type = PhysicsObjectType::DYNAMIC,
		CollisionGroup group = CollisionGroup::Default,
		CollisionGroup mask = CollisionGroup::Default,
		float density = 1.0f);

	std::shared_ptr<PhysicsObject> CreateTriangleMesh(
		const PxVec3& position,
		const std::vector<PxVec3>& vertices,
		const std::vector<uint32_t>& indices,
		PhysicsObjectType type = PhysicsObjectType::STATIC,
		CollisionGroup group = CollisionGroup::Default,
		CollisionGroup mask = CollisionGroup::Default);

	std::shared_ptr<PhysicsObject> CreateGroundPlane();

	// 캐릭터 컨트롤러 관련 함수 추가
	std::shared_ptr<PhysicsObject> CreateCapsuleController(
		const PxVec3& position,
		float radius,
		float height,
		CollisionGroup group = CollisionGroup::Default,
		CollisionGroup mask = CollisionGroup::Default);

	// 장애물 관련 함수
	PxObstacleHandle AddObstacle(const PxVec3& position,
		const PxVec3& dimensions,
		const PxQuat& rotation = PxQuat(PxIdentity));
	void RemoveObstacle(PxObstacleHandle handle);
	void ClearObstacles();

	static PxFilterData CreateFilterData(
		CollisionGroup group,
		CollisionGroup mask = CollisionGroup::Default);

	// 물리 객체 관리
	const std::vector<std::shared_ptr<PhysicsObject>>& GetPhysicsObjects() const {
		return m_physicsObjects;
	}

	// PhysX 주요 컴포넌트 접근자
	PxPhysics* GetPhysics() const { return m_physics; }
	PxScene* GetScene() const { return m_scene; }
	PxControllerManager* GetControllerManager() const { return m_controllerManager; }
	PxObstacleContext* GetObstacleContext() const { return m_obstacleContext; }

	// 컨트롤러 충돌 콜백 접근자
	CharacterControllerHitReport* GetControllerHitReport() const {
		return m_controllerHitReport.get();
	}

private:
	PxDefaultAllocator m_allocator;
	PxDefaultErrorCallback m_errorCallback;
	PxFoundation* m_foundation;
	PxPhysics* m_physics;
	PxDefaultCpuDispatcher* m_dispatcher;
	PxScene* m_scene;
	PxMaterial* m_defaultMaterial;
	PxPvd* m_pvd; // PhysX Visual Debugger

	PxControllerManager* m_controllerManager = nullptr;
	PxObstacleContext* m_obstacleContext = nullptr;

	// 객체 컨테이너
	std::vector<std::shared_ptr<PhysicsObject>> m_physicsObjects;

	// 장애물 컨테이너
	std::vector<std::pair<PxObstacleHandle, std::unique_ptr<PxBoxObstacle>>> m_obstacles;

	// 정적/동적 액터 충돌 콜백
	std::unique_ptr<ContactReportCallback> m_contactCallback;

	// 컨트롤러 충돌 콜백
	std::unique_ptr<CharacterControllerHitReport> m_controllerHitReport;

	// 초기화 헬퍼 함수들
	bool CreateFoundation();
	bool CreatePhysics();
	bool CreateScene();
	bool CreateDefaultMaterial();
	bool CreateControllerManager();
	bool CreateObstacleContext();
	void SetupDebugger();

public:
	// 충돌 필터링 설정을 위한 메서드
	static PxFilterFlags CustomFilterShader(
		PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);
};

