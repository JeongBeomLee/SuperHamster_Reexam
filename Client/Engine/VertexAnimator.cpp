#include "pch.h"
#include "VertexAnimator.h"
#include "Timer.h"
#include "Transform.h"

VertexAnimator::VertexAnimator() : Component(COMPONENT_TYPE::VERTEX_ANIMATOR)
{
    _updateTime = 0.f;
    _clipIndex = 0;
    _frame = 0;
    _nextFrame = 0;
    _frameRatio = 0.f;
    _speed = 1.f;
    _isPlaying = true;
}


VertexAnimator::~VertexAnimator()
{
}

void VertexAnimator::SetAnimClip(const vector<AnimClipInfo>* animClips)
{
    _animClips = animClips;
}

void VertexAnimator::Play()
{
    if (_animClips == nullptr || _animClips->empty())
		return;

	_isPlaying = true;
}

void VertexAnimator::Stop()
{
    if (_animClips == nullptr || _animClips->empty())
        return;

	_isPlaying = false;
}

void VertexAnimator::FinalUpdate()
{
    if (_animClips == nullptr || _animClips->empty() || !_isPlaying)
        return;

    const AnimClipInfo& currentClip = (*_animClips)[_clipIndex];

    float duration = currentClip.duration;

    _updateTime += DELTA_TIME * _speed;
    if (_updateTime > duration)
    {
        _updateTime = 0.f;
        _frame = 0;
        _nextFrame = 1;
    }

    const int32 ratio = static_cast<int32>(currentClip.frameCount / currentClip.duration);
    _frame = static_cast<int32>(_updateTime * ratio);
    _frame = min(_frame, currentClip.vertexKeyFrames.size() - 1);
    _nextFrame = min(_frame + 1, currentClip.vertexKeyFrames.size() - 1);
    _frameRatio = static_cast<float>(_frame - _frame);

    // 각 정점에 대한 현재 프레임과 다음 프레임의 위치, 스케일, 회전(오일러 각) 선형 보간
    for (size_t vertexIndex = 0; vertexIndex < currentClip.vertexKeyFrames[_frame].size(); ++vertexIndex)
    {
        const auto& currentFrame = currentClip.vertexKeyFrames[_frame][vertexIndex];
        const auto& nextFrame = currentClip.vertexKeyFrames[_nextFrame][vertexIndex];

        // 위치 선형 보간
        Vec3 interpolatedTranslation = XMVectorLerp(XMLoadFloat3(&currentFrame.translate), XMLoadFloat3(&nextFrame.translate), _frameRatio);

        // 스케일 선형 보간
        //Vec3 interpolatedScale = Lerp(currentFrame.scale, nextFrame.scale, _frameRatio);
        Vec3 interpolatedScale = XMVectorLerp(XMLoadFloat3(&currentFrame.scale), XMLoadFloat3(&nextFrame.scale), _frameRatio);

        // 오일러 회전 선형 보간
        Vec4 interpolatedRotation = Slerp(currentFrame.rotation, nextFrame.rotation, _frameRatio);

        GetTransform()->SetLocalPosition(interpolatedTranslation);
        GetTransform()->SetLocalScale(Vec3(interpolatedScale.x, interpolatedScale.z, interpolatedScale.y));
        interpolatedRotation.x -= XM_PIDIV2;
        GetTransform()->SetLocalRotation(Vec3(interpolatedRotation.x, interpolatedRotation.y, interpolatedRotation.z));
    }
}