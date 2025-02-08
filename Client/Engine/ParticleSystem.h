#pragma once
#include "Component.h"

class Material;
class Mesh;
class StructuredBuffer;

//struct ParticleInfo
//{
//	Vec3	worldPos;
//	float	curTime;
//	Vec3	worldDir;
//	float	lifeTime;
//	int32	alive;
//	int32	padding[3];
//};
//
//struct ComputeSharedInfo
//{
//	int32 addCount;
//	int32 padding[3];
//};

class ParticleSystem : public Component
{
public:
	// ����Ʈ ���� ����
	enum class EffectType
	{
		DEFAULT,
		MUZZLE_FLASH,
		SHOCKWAVE,
		COLLISION
	};

	// ����Ʈ �Ķ���� ����ü
	struct EffectDesc
	{
		EffectType type = EffectType::DEFAULT;
		float duration = 1.0f;       // ����Ʈ ���� �ð�
		float startScale = 1.0f;     // ���� ũ��
		float endScale = 1.0f;       // ���� ũ��
		Vec4 color = Vec4(1.f);      // ����
		Vec3 direction = Vec3(0.f);  // ���� (�ʿ��� ���)
		shared_ptr<Material> material = nullptr; // ����Ʈ�� ��Ƽ����

		// ������ ��ƼŬ �ý��� �Ķ���͵��� ���� ���ʿ��ϹǷ� �ּ� ó��
		/*float minLifeTime = 0.5f;
		float maxLifeTime = 1.0f;
		float minSpeed = 100.f;
		float maxSpeed = 50.f;
		float _createInterval = 0.005f;
		float _accTime = 0.f;*/
	};

public:
	ParticleSystem();
	virtual ~ParticleSystem();

public:
	void Update() override;
	void FinalUpdate() override;
	void Render();

	void Play(const EffectDesc& desc);
	void Stop();

	bool IsPlaying() const { return _isPlaying; }
	float GetElapsedTime() const { return _elapsedTime; }
	float GetProgress() const { return _elapsedTime / _desc.duration; }

private:
	// ���� ����Ʈ ǥ���� ���� ��� ������
	shared_ptr<Mesh>			_mesh;  // ����Ʈ �޽� (�簢��)
	shared_ptr<Material>		_material; // ���� ������� ��Ƽ����

	EffectDesc _desc;               // ���� ������� ����Ʈ ����
	float _elapsedTime = 0.f;      // ��� �ð�
	bool _isPlaying = false;       // ��� ������ ����

	// ��ǻƮ ���̴� ����
	/*shared_ptr<StructuredBuffer> _particleBuffer;
	shared_ptr<StructuredBuffer> _computeSharedBuffer;
	shared_ptr<Material> _computeMaterial;
	uint32 _maxParticle = 1000;
	float _createInterval = 0.005f;
	float _accTime = 0.f;
	*/
};
