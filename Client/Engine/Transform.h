#pragma once
#include "Component.h"

class Transform : public Component
{
public:
	Transform();
	virtual ~Transform();

	virtual void FinalUpdate() override;
	void PushData();

public:
	// Parent ����
	const Vec3& GetLocalPosition() { return _localPosition; }
	const Vec3& GetLocalRotation() { return _localRotation; }
	const Vec3& GetLocalScale() { return _localScale; }

	// TEMP
	float GetBoundingSphereRadius() { return max(max(_localScale.x, _localScale.y), _localScale.z); }

	const Matrix& GetLocalToWorldMatrix() { return _matWorld; }
	Vec3 GetWorldPosition() { return _matWorld.Translation(); }

	Vec3 GetRight() { return _matWorld.Right(); }
	Vec3 GetUp() { return _matWorld.Up(); }
	Vec3 GetLook() { return _matWorld.Backward(); }
	Vec3 GetForward() { return _matWorld.Forward(); }

	void SetLocalPosition(const Vec3& position);
	void SetLocalRotation(const Vec3& rotation);
	void SetLocalScale(const Vec3& scale);

	void LookAt(const Vec3& dir);

	static bool CloseEnough(const float& a, const float& b, const float& epsilon = std::numeric_limits<float>::epsilon());
	static Vec3 DecomposeRotationMatrix(const Matrix& rotation);

public:
	void SetParent(shared_ptr<Transform> parent) { _parent = parent; }
	weak_ptr<Transform> GetParent() { return _parent; }
	void AttachToBone(const std::shared_ptr<GameObject>& parent, const std::wstring& boneName);
	const Matrix& GetBoneMatrix(const std::wstring& boneName) const;

private:
	// Parent ����
	Vec3 _localPosition = {};
	Vec3 _localRotation = {};
	Vec3 _localScale = { 1.f, 1.f, 1.f };

	Matrix _matLocal= {};
	Matrix _matWorld = {};

	weak_ptr<Transform> _parent;
	weak_ptr<GameObject> _parentObject;
	wstring _attachBoneName;
};

