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

ParticleSystem::ParticleSystem(const std::wstring& textureName, const std::wstring& texturePath, const std::wstring& shaderName) : Component(COMPONENT_TYPE::PARTICLE_SYSTEM)
{
	_particleBuffer = make_shared<StructuredBuffer>();
	_particleBuffer->Init(sizeof(ParticleInfo), _maxParticle);

	_computeSharedBuffer = make_shared<StructuredBuffer>();
	_computeSharedBuffer->Init(sizeof(ComputeSharedInfo), 1);

	_mesh = GET_SINGLE(Resources)->LoadPointMesh();
	SetShader(shaderName);
	SetTexture(textureName, texturePath);
	_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeParticle");

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

