#pragma once
#include "Component.h"
#include "Object.h"

class Transform;
class MeshRenderer;
class Camera;
class Light;
class MonoBehaviour;
class ParticleSystem;
class Terrain;
class BaseCollider;
class Animator;
class CharacterController;
class PlayerMovement;
class PhysicsBody;
class TransformAnimator;

class GameObject : public Object, public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	virtual ~GameObject();

	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FinalUpdate();

	shared_ptr<Component> GetFixedComponent(COMPONENT_TYPE type);

	shared_ptr<Transform> GetTransform();
	shared_ptr<MeshRenderer> GetMeshRenderer();
	shared_ptr<Camera> GetCamera();
	shared_ptr<Light> GetLight();
	shared_ptr<ParticleSystem> GetParticleSystem();
	shared_ptr<Terrain> GetTerrain();
	shared_ptr<BaseCollider> GetCollider();
	shared_ptr<Animator> GetAnimator();
	shared_ptr<CharacterController> GetCharacterController();
	shared_ptr<PlayerMovement> GetPlayerMovement();
	shared_ptr<PhysicsBody> GetPhysicsBody();
	shared_ptr<TransformAnimator> GetTransformAnimator();

	void AddComponent(shared_ptr<Component> component);

	template<typename T>
	shared_ptr<T> GetMonoBehaviour() {
		static_assert(std::is_base_of<MonoBehaviour, T>::value, "T must inherit from MonoBehaviour");
		for (const auto& script : _scripts) {
			if (auto castedScript = dynamic_pointer_cast<T>(script)) {
				return castedScript;
			}
		}
		return nullptr;
	}

	const vector<shared_ptr<MonoBehaviour>>& GetAllMonoBehaviours() const { return _scripts; }

	void SetCheckFrustum(bool checkFrustum) { _checkFrustum = checkFrustum; }
	bool GetCheckFrustum() { return _checkFrustum; }

	void SetLayerIndex(uint8 layer) { _layerIndex = layer; }
	uint8 GetLayerIndex() { return _layerIndex; }

	/* static한 오브젝트는 그림자 렌더링 대상에서 제외됨. */
	void SetStatic(bool flag) { _static = flag; }
	bool IsStatic() { return _static; } 

	void SetActive(bool active) { _isActive = active; }
	bool IsActive() { return _isActive; }

	void AttachToBone(const std::shared_ptr<GameObject>& parent, const std::wstring& boneName);

private:
	array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;
	vector<shared_ptr<MonoBehaviour>> _scripts;

	bool _checkFrustum = true;
	uint8 _layerIndex = 0;
	bool _static = true;
	bool _isActive = true;
};

