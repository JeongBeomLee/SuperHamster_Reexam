#pragma once
#include "Component.h"
#include "FBXLoader.h"

class TransformAnimator : public Component
{
public:
    TransformAnimator();
    virtual ~TransformAnimator();

    virtual void FinalUpdate() override;

    void SetTransformAnimClip(const vector<shared_ptr<TransformAnimClipInfo>>* clips);
    void Play(uint32 index = 0);
    void Stop() { _isPlaying = false; }
    void Resume() { _isPlaying = true; }

    bool IsFinished() const { return _animFinished; }
    float GetProgress() const;
    bool IsPlaying() const { return _isPlaying; }

    void SetLoop(bool loop) { _loop = loop; }
    bool IsLooping() const { return _loop; }

private:
    void UpdateTransform();

private:
    const vector<shared_ptr<TransformAnimClipInfo>>* _animClips = nullptr;
    float _updateTime = 0.f;
    uint32 _clipIndex = 0;
    uint32 _frameIndex = 0;
    uint32 _nextFrameIndex = 0;
    float _frameRatio = 0.f;
    bool _isPlaying = true;
    bool _animFinished = false;
    bool _loop = false;  // 반복 재생 여부
};
