#pragma once
#include "Component.h"
#include "StructuredBuffer.h"
#include "Resources.h"
class Material;
class Mesh;
class StructuredBuffer;
class Texture;

struct ParticleInfo
{
	Vec3	worldPos;
	float	curTime;
	Vec3	worldDir;
	float	lifeTime;
	int32	alive;
	int32	padding[3];
};

struct ComputeSharedInfo
{
	int32 addCount;
	int32 padding[3];
};

class ParticleSystem : public Component
{
public:
	ParticleSystem();
	virtual ~ParticleSystem();
	ParticleSystem(const std::wstring& textureName, const std::wstring& texturePath, const std::wstring& shaderName);

public:
	virtual void FinalUpdate();
	void Render();
	void SetShader(const std::wstring& shaderName);
	void SetTexture(const std::wstring& textureName, const std::wstring& texturePath);

public:
	virtual void Load(const wstring& path) override { } 
	virtual void Save(const wstring& path) override { }
	void SetSpeed(float maxSpeed, float minSpeed) { _maxSpeed = maxSpeed; _minSpeed = minSpeed;  }
	void SetLifeTime(float maxLifeTime, float minLifeTime) { _maxLifeTime = maxLifeTime; _minLifeTime = minLifeTime; }
	void SetScale(float startScale, float endScale) { _startScale = startScale; _endScale = endScale; }
	void SetMaxParticle(uint32 maxParticle) { _maxParticle = maxParticle; }
	void SetaccTime(float accTime) { _accTime = accTime; }

protected:
	shared_ptr<StructuredBuffer>	_particleBuffer;
	shared_ptr<StructuredBuffer>	_computeSharedBuffer;
	uint32							_maxParticle = 1000;

	shared_ptr<Material>		_computeMaterial;
	shared_ptr<Material>		_material;
	shared_ptr<Mesh>			_mesh;

	float				_createInterval = 0.005f; // 200fps
	float				_accTime = 0.f;

	float				_minLifeTime = 0.5f;
	float				_maxLifeTime = 1.f;
	float				_minSpeed = 100;
	float				_maxSpeed = 50;
	float				_startScale = 10.f;
	float				_endScale = 5.f;
};


