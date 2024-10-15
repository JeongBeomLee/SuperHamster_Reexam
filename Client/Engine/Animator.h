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

public:
	virtual void FinalUpdate() override;

private:
	const vector<BoneInfo>*			_bones;
	const vector<AnimClipInfo>*		_animClips;
	vector<Matrix>					_boneFinalMatrices;

	float							_updateTime = 0.f;
	int32							_clipIndex = 0;
	int32							_frame = 0;
	int32							_nextFrame = 0;
	float							_frameRatio = 0;

	shared_ptr<Material>			_computeMaterial;
	shared_ptr<StructuredBuffer>	_boneFinalMatrix;  // Ư�� �������� ���� ���
	bool							_boneFinalUpdated = false;
};
