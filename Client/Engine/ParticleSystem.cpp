#include "pch.h"
#include "ParticleSystem.h"
#include "StructuredBuffer.h"
#include "Mesh.h"
#include "Resources.h"
#include "Transform.h"
#include "Timer.h"

ParticleSystem::ParticleSystem() : Component(COMPONENT_TYPE::PARTICLE_SYSTEM)
{
	_mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Update()
{
    if (!_isPlaying)
        return;

    _elapsedTime += DELTA_TIME;

    // ���� �ð��� ������ ����Ʈ ����
    if (_elapsedTime >= _desc.duration)
    {
        Stop();
        return;
    }

    // ���൵ ��� (0~1)
    float progress = GetProgress();

    // ���� ������ ��� (���� ����)
    float currentScale = std::lerp(_desc.startScale, _desc.endScale, progress);
    GetTransform()->SetLocalScale(Vec3(currentScale));

    // ��Ƽ���� �Ķ���� ������Ʈ
    if (_material)
    {
        _material->SetFloat(0, _elapsedTime);         // ��� �ð�
        _material->SetFloat(1, _desc.duration);       // �� ���� �ð�
        _material->SetFloat(2, progress);             // ���൵
        _material->SetVec4(0, _desc.color);          // ����
    }
}

void ParticleSystem::FinalUpdate()
{
    // �ʿ��� ��� ���⼭ �߰����� ������Ʈ ����
}

void ParticleSystem::Render()
{
    if (!_isPlaying || !_material)
        return;

    GetTransform()->PushData();
    _material->PushGraphicsData();
    _mesh->Render();
}

void ParticleSystem::Play(const EffectDesc& desc)
{
    _desc = desc;
    _material = desc.material;
    _elapsedTime = 0.f;
    _isPlaying = true;

    // ����Ʈ ���� �� �ʱ� ������ ����
    GetTransform()->SetLocalScale(Vec3(_desc.startScale));
}

void ParticleSystem::Stop()
{
	_isPlaying = false;
	_elapsedTime = 0.f;
}
