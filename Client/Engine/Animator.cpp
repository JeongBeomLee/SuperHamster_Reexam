#include "pch.h"
#include "Animator.h"
#include "Timer.h"
#include "Resources.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "StructuredBuffer.h"

Animator::Animator() : Component(COMPONENT_TYPE::ANIMATOR)
{
	_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeAnimation");
	_boneFinalMatrix = make_shared<StructuredBuffer>();
}

Animator::~Animator()
{
}

void Animator::FinalUpdate()
{
	_updateTime += DELTA_TIME;

	const AnimClipInfo& animClip = _animClips->at(_clipIndex);
	// 애니메이션 종료 체크
    _animationFinished = (_updateTime > animClip.duration);
    
    if (_animationFinished) {
        _updateTime = 0.f;
    }

	const int32 ratio = static_cast<int32>(animClip.frameCount / animClip.duration);
	_frame = static_cast<int32>(_updateTime * ratio);
	_frame = min(_frame, animClip.frameCount - 1);
	_nextFrame = min(_frame + 1, animClip.frameCount - 1);
	_frameRatio = static_cast<float>(_frame - _frame);

	UpdateBoneFinalMatrices();
}

void Animator::SetAnimClip(const vector<AnimClipInfo>* animClips)
{
	_animClips = animClips;
}

void Animator::PushData()
{
	uint32 boneCount = static_cast<uint32>(_bones->size());
	if (_boneFinalMatrix->GetElementCount() < boneCount)
		_boneFinalMatrix->Init(sizeof(Matrix), boneCount);

	// Compute Shader
	shared_ptr<Mesh> mesh = GetGameObject()->GetMeshRenderer()->GetMesh();
	mesh->GetBoneFrameDataBuffer(_clipIndex)->PushComputeSRVData(SRV_REGISTER::t8);
	mesh->GetBoneOffsetBuffer()->PushComputeSRVData(SRV_REGISTER::t9);

	_boneFinalMatrix->PushComputeUAVData(UAV_REGISTER::u0);

	_computeMaterial->SetInt(0, boneCount);
	_computeMaterial->SetInt(1, _frame);
	_computeMaterial->SetInt(2, _nextFrame);
	_computeMaterial->SetFloat(0, _frameRatio);

	uint32 groupCount = (boneCount / 256) + 1;
	_computeMaterial->Dispatch(groupCount, 1, 1);

	// Graphics Shader
	_boneFinalMatrix->PushGraphicsData(SRV_REGISTER::t7);
}

void Animator::Play(uint32 idx)
{
	assert(idx < _animClips->size());
	_clipIndex = idx;
	_updateTime = 0.f;
}

void Animator::UpdateBoneFinalMatrices()
{
	if (_clipIndex == -1 || _bones == nullptr || _animClips == nullptr)
		return;

	const AnimClipInfo& animClip = _animClips->at(_clipIndex);
	const vector<BoneInfo>& bones = *_bones;
	_boneFinalMatrices.resize(bones.size());

	// 현재 프레임과 다음 프레임을 찾아서 bone의 최종 행렬을 계산
	for (size_t i = 0; i < bones.size(); ++i) {
		const BoneInfo& bone = bones[i];
		const vector<KeyFrameInfo>& keyFrames = animClip.keyFrames[i];

		if (_frame >= keyFrames.size())
			continue;

		const KeyFrameInfo& frameA = keyFrames[_frame];
		const KeyFrameInfo& frameB = keyFrames[_nextFrame];

		Vec3 scale = Vec3::Lerp(frameA.scale, frameB.scale, _frameRatio);
		SimpleMath::Quaternion rotation = SimpleMath::Quaternion::Slerp(frameA.rotation, frameB.rotation, _frameRatio);
		Vec3 translation = Vec3::Lerp(frameA.translate, frameB.translate, _frameRatio);

		Matrix matBone = Matrix::CreateScale(scale) * Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(translation);

		_boneFinalMatrices[i] = matBone;
	}
}

float Animator::GetCurrentAnimationDuration() const
{
	if (!_animClips || _clipIndex >= _animClips->size())
		return 0.0f;

	return _animClips->at(_clipIndex).duration;
}

float Animator::GetCurrentAnimationProgress() const
{
	if (!_animClips || _clipIndex >= _animClips->size())
		return 0.0f;

	return _updateTime / _animClips->at(_clipIndex).duration;
}

const AnimClipInfo* Animator::GetCurrentAnimClip() const
{
	if (!_animClips || _clipIndex >= _animClips->size())
		return nullptr;

	return &_animClips->at(_clipIndex);
}
