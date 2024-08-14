#pragma once
#include "MonoBehaviour.h"

class PlayerCameraScript : public MonoBehaviour
{
public:
	PlayerCameraScript();
	virtual ~PlayerCameraScript();

	virtual void LateUpdate() override;

	void SetRotationX(float x)			{ _RotationX = x; }
	void SetRotationY(float y)			{ _RotationY = y; }
	void SetDistance(float distance)	{ _Distance = distance; }
	void SetHeight(float height)		{ _Height = height; }

private:
	float _speed		= 500.f;
	float _RotationX	= 45.f;
	float _RotationY	= -180.f;
	float _Distance		= 1000.f;
	float _Height		= 250.f;
};

