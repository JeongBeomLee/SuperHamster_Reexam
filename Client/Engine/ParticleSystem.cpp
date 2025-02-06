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

    // 지속 시간이 끝나면 이펙트 정지
    if (_elapsedTime >= _desc.duration)
    {
        Stop();
        return;
    }

    // 진행도 계산 (0~1)
    float progress = GetProgress();

    // 현재 스케일 계산 (선형 보간)
    float currentScale = std::lerp(_desc.startScale, _desc.endScale, progress);
    GetTransform()->SetLocalScale(Vec3(currentScale));

    // 머티리얼 파라미터 업데이트
    if (_material)
    {
        _material->SetFloat(0, _elapsedTime);         // 경과 시간
        _material->SetFloat(1, _desc.duration);       // 총 지속 시간
        _material->SetFloat(2, progress);             // 진행도
        _material->SetVec4(0, _desc.color);          // 색상
    }
}

void ParticleSystem::FinalUpdate()
{
    // 필요한 경우 여기서 추가적인 업데이트 수행
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

    // 이펙트 시작 시 초기 스케일 설정
    GetTransform()->SetLocalScale(Vec3(_desc.startScale));
}

void ParticleSystem::Stop()
{
	_isPlaying = false;
	_elapsedTime = 0.f;
}
