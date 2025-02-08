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
	// 이펙트 종류 정의
	enum class EffectType
	{
		DEFAULT,
		MUZZLE_FLASH,
		SHOCKWAVE,
		COLLISION
	};

	// 이펙트 파라미터 구조체
	struct EffectDesc
	{
		EffectType type = EffectType::DEFAULT;
		float duration = 1.0f;       // 이펙트 지속 시간
		float startScale = 1.0f;     // 시작 크기
		float endScale = 1.0f;       // 종료 크기
		Vec4 color = Vec4(1.f);      // 색상
		Vec3 direction = Vec3(0.f);  // 방향 (필요한 경우)
		shared_ptr<Material> material = nullptr; // 이펙트별 머티리얼

		// 기존의 파티클 시스템 파라미터들은 당장 불필요하므로 주석 처리
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
	// 단일 이펙트 표현을 위한 멤버 변수들
	shared_ptr<Mesh>			_mesh;  // 이펙트 메시 (사각형)
	shared_ptr<Material>		_material; // 현재 사용중인 머티리얼

	EffectDesc _desc;               // 현재 재생중인 이펙트 정보
	float _elapsedTime = 0.f;      // 경과 시간
	bool _isPlaying = false;       // 재생 중인지 여부

	// 컴퓨트 셰이더 관련
	/*shared_ptr<StructuredBuffer> _particleBuffer;
	shared_ptr<StructuredBuffer> _computeSharedBuffer;
	shared_ptr<Material> _computeMaterial;
	uint32 _maxParticle = 1000;
	float _createInterval = 0.005f;
	float _accTime = 0.f;
	*/
};
