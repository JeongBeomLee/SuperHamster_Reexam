#include "pch.h"
#include "TransformAnimator.h"
#include "Timer.h"
#include "Transform.h"

TransformAnimator::TransformAnimator() : Component(COMPONENT_TYPE::TRANSFORM_ANIMATOR)
{
}

TransformAnimator::~TransformAnimator()
{
}

void TransformAnimator::FinalUpdate()
{
    if (!_animClips || !_isPlaying || _animClips->empty())
        return;

    const auto& currentClip = _animClips->at(_clipIndex);
    if (!currentClip || currentClip->keyFrames.empty())
        return;

    _updateTime += DELTA_TIME;
    _animFinished = (_updateTime >= currentClip->duration);

    if (_animFinished) {
        if (_loop) {
            _updateTime = 0.f;
            _frameIndex = 0;
            _nextFrameIndex = 1;
            _frameRatio = 0.f;
            _animFinished = false;  // 루프 시 finished 상태 초기화
        }
        else {
            _isPlaying = false;  // 루프가 아닐 경우 재생 중지
            _updateTime = currentClip->duration;  // 마지막 프레임 유지
        }
    }

    UpdateTransform();
}

void TransformAnimator::UpdateTransform()
{
    const auto& currentClip = _animClips->at(_clipIndex);
    const auto& keyFrames = currentClip->keyFrames;
    const size_t frameCount = keyFrames.size();

    if (frameCount < 2)
        return;

    // 균일 분포라고 가정하고 인덱스를 산출하는 기존 방식
    float framePerSec = static_cast<float>(frameCount) / currentClip->duration;
    _frameIndex = static_cast<uint32>(_updateTime * framePerSec);

    if (_loop) {
        // 루프 모드인 경우, 마지막 키프레임일 때는 다음 프레임을 첫 번째 키프레임으로 지정
        if (_frameIndex >= frameCount - 1)
            _frameIndex = frameCount - 1;
        _nextFrameIndex = (_frameIndex == frameCount - 1) ? 0 : _frameIndex + 1;
    }
    else {
        _frameIndex = min(_frameIndex, static_cast<uint32>(frameCount - 1));
        _nextFrameIndex = min(_frameIndex + 1, static_cast<uint32>(frameCount - 1));
    }

    const auto& frame = keyFrames[_frameIndex];
    const auto& nextFrame = keyFrames[_nextFrameIndex];

    float deltaTime = 0.f;
    if (_loop && _frameIndex == frameCount - 1) {
        // 루프일 경우: 마지막 키프레임과 첫 키프레임 사이의 시간 간격 계산
        // (일반적으로 첫 키프레임의 time은 0으로 가정)
        deltaTime = (currentClip->duration - frame.time.GetSecondDouble()) +
            (keyFrames[0].time.GetSecondDouble());
    }
    else {
        deltaTime = (nextFrame.time - frame.time).GetSecondDouble();
    }

    // 보간 비율 계산:
    // (_updateTime – frame.time)가 last->first 구간에 걸친 경우에도 올바르게 계산됨.
    _frameRatio = deltaTime > 0.f ?
        static_cast<float>((_updateTime - frame.time.GetSecondDouble()) / deltaTime) : 0.f;

    // 보간 비율을 0~1 범위로 클램핑
    if (_frameRatio > 1.f)
        _frameRatio = 1.f;
    else if (_frameRatio < 0.f)
        _frameRatio = 0.f;

    // Transform 업데이트 (필요에 따라 회전은 쿼터니언 보간으로 대체할 수 있음)
    auto transform = GetTransform();
    Vec3 pos = Vec3::Lerp(frame.localTranslation, nextFrame.localTranslation, _frameRatio);
    Vec3 scale = Vec3::Lerp(frame.localScale, nextFrame.localScale, _frameRatio);
    Vec3 rot = Vec3::Lerp(frame.localRotation, nextFrame.localRotation, _frameRatio);

    transform->SetLocalPosition(_startPos + pos);
    transform->SetLocalScale(_startScale * scale);
    transform->SetLocalRotation(_startRot + rot);
}

void TransformAnimator::SetTransformAnimClip(const vector<shared_ptr<TransformAnimClipInfo>>* clips)
{
	_animClips = clips;
}

void TransformAnimator::Play(uint32 index)
{
    if (!_animClips || index >= _animClips->size())
        return;

    _clipIndex = index;
    _updateTime = 0.f;
    _frameIndex = 0;
    _nextFrameIndex = 1;
    _frameRatio = 0.f;
    _isPlaying = true;
    _animFinished = false;
}

float TransformAnimator::GetProgress() const
{
    if (!_animClips || _animClips->empty())
        return 0.f;

    const auto& currentClip = _animClips->at(_clipIndex);
    if (!currentClip)
        return 0.f;

    return _updateTime / currentClip->duration;
}