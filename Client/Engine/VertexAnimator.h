#pragma once

#include "Component.h"
#include "Mesh.h"
class Material;
class StructuredBuffer;

class VertexAnimator : public Component
{
public:
    VertexAnimator();
    virtual ~VertexAnimator();

public:
    void SetAnimClip(const vector<AnimClipInfo>* animClips);

    int32 GetAnimCount() const { return static_cast<int32>(_animClips->size()); }
    int32 GetCurrentClipIndex() const { return _clipIndex; }
    void Play();
    void Stop(); 
    void SetSpeed(float speed) { _speed = speed; }
    bool IsPlaying() const { return _isPlaying; }

public:
    virtual void FinalUpdate() override;

private:
    const vector<AnimClipInfo>* _animClips;

    float _updateTime = 0.f;
    int32 _clipIndex = 0;
    int32 _frame = 0;
    int32 _nextFrame = 0;
    float _frameRatio = 0;
    float _speed = 1.f;
    bool _isPlaying = false;
};
