#include "pch.h"
#include "Transform.h"
#include "Engine.h"
#include "Camera.h"
#include "Animator.h"
#include "GameObject.h"

Transform::Transform() : Component(COMPONENT_TYPE::TRANSFORM)
{

}

Transform::~Transform()
{

}

void Transform::FinalUpdate()
{
	Matrix matScale = Matrix::CreateScale(_localScale);
	Matrix matRotation = Matrix::CreateRotationX(_localRotation.x);
	matRotation *= Matrix::CreateRotationY(_localRotation.y);
	matRotation *= Matrix::CreateRotationZ(_localRotation.z);
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

	_matLocal = matScale * matRotation * matTranslation;
	_matWorld = _matLocal;

	shared_ptr<Transform> parent = GetParent().lock();
	if (parent != nullptr) {
		if (!_attachBoneName.empty()) { 
			//_matWorld = GetBoneMatrix(_attachBoneName);	// 붙일 본의 행렬을 가져와서 곱해준다.
			Matrix attachBoneMatrix = GetBoneMatrix(_attachBoneName);
			_matWorld *= attachBoneMatrix;	// 붙일 본의 행렬을 가져와서 곱해준다.
		}
		Matrix parentWorldMatrix = parent->GetLocalToWorldMatrix();
		_matWorld *= parentWorldMatrix;	// 그 후 부모의 행렬을 곱해줘서 부모의 공간으로 변환한다.
	}
}

void Transform::PushData()
{
	TransformParams transformParams = {};
	transformParams.matWorld = _matWorld;
	transformParams.matView = Camera::S_MatView;
	transformParams.matProjection = Camera::S_MatProjection;
	transformParams.matWV = _matWorld * Camera::S_MatView;
	transformParams.matWVP = _matWorld * Camera::S_MatView * Camera::S_MatProjection;
	transformParams.matViewInv = Camera::S_MatView.Invert();

	CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushGraphicsData(&transformParams, sizeof(transformParams));
}

void Transform::LookAt(const Vec3& dir)
{
	Vec3 front = dir;
	front.Normalize();

	Vec3 right = Vec3::Up.Cross(dir);
	if (right == Vec3::Zero)
		right = Vec3::Forward.Cross(dir);

	right.Normalize();

	Vec3 up = front.Cross(right);
	up.Normalize();

	Matrix matrix = XMMatrixIdentity();
	matrix.Right(right);
	matrix.Up(up);
	matrix.Backward(front);

	_localRotation = DecomposeRotationMatrix(matrix);
}

bool Transform::CloseEnough(const float& a, const float& b, const float& epsilon)
{
	return (epsilon > std::abs(a - b));
}

Vec3 Transform::DecomposeRotationMatrix(const Matrix& rotation)
{
	Vec4 v[4];
	XMStoreFloat4(&v[0], rotation.Right());
	XMStoreFloat4(&v[1], rotation.Up());
	XMStoreFloat4(&v[2], rotation.Backward());
	XMStoreFloat4(&v[3], rotation.Translation());

	Vec3 ret;
	if (CloseEnough(v[0].z, -1.0f))
	{
		float x = 0;
		float y = XM_PI / 2;
		float z = x + atan2(v[1].x, v[2].x);
		ret = Vec3{ x, y, z };
	}
	else if (CloseEnough(v[0].z, 1.0f))
	{
		float x = 0;
		float y = -XM_PI / 2;
		float z = -x + atan2(-v[1].x, -v[2].x);
		ret = Vec3{ x, y, z };
	}
	else
	{
		float y1 = -asin(v[0].z);
		float y2 = XM_PI - y1;

		float x1 = atan2f(v[1].z / cos(y1), v[2].z / cos(y1));
		float x2 = atan2f(v[1].z / cos(y2), v[2].z / cos(y2));

		float z1 = atan2f(v[0].y / cos(y1), v[0].x / cos(y1));
		float z2 = atan2f(v[0].y / cos(y2), v[0].x / cos(y2));

		if ((std::abs(x1) + std::abs(y1) + std::abs(z1)) <= (std::abs(x2) + std::abs(y2) + std::abs(z2)))
		{
			ret = Vec3{ x1, y1, z1 };
		}
		else
		{
			ret = Vec3{ x2, y2, z2 };
		}
	}

	return ret;
}

void Transform::AttachToBone(const std::shared_ptr<GameObject>& parent, const std::wstring& boneName)
{
	_parentObject = parent;
	_attachBoneName = boneName;
}

const Matrix& Transform::GetBoneMatrix(const std::wstring& boneName) const
{
	static Matrix identityMatrix = Matrix::Identity;
	shared_ptr<GameObject> parentObj = _parentObject.lock();
	if (!parentObj)
		return identityMatrix;

	shared_ptr<Animator> animator = parentObj->GetAnimator();
	if (!animator)
		return identityMatrix;

	const vector<BoneInfo>* boneInfos = animator->GetBones();
	if (!boneInfos)
		return identityMatrix;

	for (size_t i = 0; i < boneInfos->size(); ++i) {
		if ((*boneInfos)[i].boneName == boneName) {
			const Matrix& boneMatrix = animator->GetBoneFinalMatrix(i);
			// 컴파일러 최적화를 방지하기 위한 volatile 사용
			volatile float checkSum = boneMatrix._11 + boneMatrix._22 + boneMatrix._33 + boneMatrix._44;
			if (checkSum == 4.0f)	// identityMatrix인지 체크
				return identityMatrix;
			return boneMatrix;
		}
	}

	return identityMatrix;
}
