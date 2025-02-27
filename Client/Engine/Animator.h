#pragma once
#include "Component.h"
#include "Mesh.h"

class Material;
class StructuredBuffer;
class Mesh;

class Animator : public Component
{
public:
	Animator();
	virtual ~Animator();

public:
	const vector<BoneInfo>* GetBones() { return _bones; }
	Matrix GetBoneFinalMatrix(uint32 idx) { return _boneFinalMatrices[idx]; }

	void SetBones(const vector<BoneInfo>* bones) { _bones = bones; }
	void SetAnimClip(const vector<AnimClipInfo>* animClips);
	void PushData();

	int32 GetAnimCount() { return static_cast<uint32>(_animClips->size()); }
	int32 GetCurrentClipIndex() { return _clipIndex; }
	void Play(uint32 idx);
	void UpdateBoneFinalMatrices();

	// 현재 애니메이션 관련 정보 조회
	bool IsAnimationFinished() const { return _animationFinished; }
	float GetCurrentAnimationDuration() const;
	float GetCurrentAnimationProgress() const;
	const AnimClipInfo* GetCurrentAnimClip() const;

public:
	virtual void FinalUpdate() override;

private:
	const vector<BoneInfo>*			_bones;
	const vector<AnimClipInfo>*		_animClips;
	vector<Matrix>					_boneFinalMatrices;

	float							_updateTime = 0.f;
	int32							_clipIndex = 0;
	int32							_frame = 0;
	bool							_animationFinished = false;
	int32							_nextFrame = 0;
	float							_frameRatio = 0;

	shared_ptr<Material>			_computeMaterial;
	shared_ptr<StructuredBuffer>	_boneFinalMatrix;  // 특정 프레임의 최종 행렬
	bool							_boneFinalUpdated = false;
};
