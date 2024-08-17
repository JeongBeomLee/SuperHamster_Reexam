#include "pch.h"
#include "ParticleSystem.h"
#include "StructuredBuffer.h"
#include "Mesh.h"
#include "Resources.h"
#include "Transform.h"
#include "Timer.h"

ParticleSystem::ParticleSystem() : Component(COMPONENT_TYPE::PARTICLE_SYSTEM)
{
	_particleBuffer = make_shared<StructuredBuffer>();
	_particleBuffer->Init(sizeof(ParticleInfo), _maxParticle);

	_computeSharedBuffer = make_shared<StructuredBuffer>();
	_computeSharedBuffer->Init(sizeof(ComputeSharedInfo), 1);

	_mesh = GET_SINGLE(Resources)->LoadPointMesh();
	_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeParticle");

	SetShader(L"Particle");
	SetTexture(L"bubble", L"..\\Resources\\Texture\\Particle\\bubble.png");
}


ParticleSystem::~ParticleSystem()
{
}

ParticleSystem::ParticleSystem(wstring name) : Component(COMPONENT_TYPE::PARTICLE_SYSTEM)
{
	_particleBuffer = make_shared<StructuredBuffer>();
	_particleBuffer->Init(sizeof(ParticleInfo), _maxParticle);

	_computeSharedBuffer = make_shared<StructuredBuffer>();
	_computeSharedBuffer->Init(sizeof(ComputeSharedInfo), 1);

	_mesh = GET_SINGLE(Resources)->LoadPointMesh();
	_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeParticle");

	if (name == L"Default") {
		SetShader(L"Particle");
		SetTexture(L"bubble", L"..\\Resources\\Texture\\Particle\\bubble.png");
		_maxParticle = 1;

		_minLifeTime = 0.5f;
		_maxLifeTime = 1.f;
		_minSpeed = 100;
		_maxSpeed = 50;
		_startScale = 0.5f;
		_endScale = 10.f;
	}

	else if (name == L"Glow") {
		SetShader(L"GlowParticle");
		SetTexture(L"glow", L"..\\Resources\\Texture\\Particle\\Star.png");
		_maxParticle = 100;

		_minLifeTime = 0.5f;
		_maxLifeTime = 1.f;
		_minSpeed = 100;
		_maxSpeed = 50;

		_startScale = 0.5f;
		_endScale = 25.f;
	}
	else if(name == L"Portal"){
		SetShader(L"PortalParticle");
		SetTexture(L"portal", L"..\\Resources\\Texture\\Particle\\layser.png");
		_maxParticle = 1000;
		_minLifeTime = 0.9f;
		_maxLifeTime = 5.f;
		_minSpeed = 100;
		_maxSpeed = 50;

		_startScale = 10.f;
		_endScale = 10.f;
	}
}




void ParticleSystem::FinalUpdate()
{
	_accTime += DELTA_TIME;

	int32 add = 0;
	if (_createInterval < _accTime)
	{
		_accTime = _accTime - _createInterval;
		add = 1;
	}

	_particleBuffer->PushComputeUAVData(UAV_REGISTER::u0);
	_computeSharedBuffer->PushComputeUAVData(UAV_REGISTER::u1);

	_computeMaterial->SetInt(0, _maxParticle);
	_computeMaterial->SetInt(1, add);

	_computeMaterial->SetVec2(1, Vec2(DELTA_TIME, _accTime));
	_computeMaterial->SetVec4(0, Vec4(_minLifeTime, _maxLifeTime, _minSpeed, _maxSpeed));

	_computeMaterial->Dispatch(1, 1, 1);
}

void ParticleSystem::Render()
{
	GetTransform()->PushData();

	_particleBuffer->PushGraphicsData(SRV_REGISTER::t9);
	_material->SetFloat(0, _startScale);
	_material->SetFloat(1, _endScale);
	_material->PushGraphicsData();

	_mesh->Render(_maxParticle);
}



void ParticleSystem::SetShader(const std::wstring& shaderName)
{
	_material = GET_SINGLE(Resources)->Get<Material>(shaderName);
}

void ParticleSystem::SetTexture(const std::wstring& textureName, const std::wstring& texturePath)
{
	shared_ptr<Texture> tex = GET_SINGLE(Resources)->Load<Texture>(textureName, texturePath);
	_material->SetTexture(0, tex);
}

