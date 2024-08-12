#include "pch.h"
#include "PlayerCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Scene.h"

PlayerCameraScript::PlayerCameraScript()
{
}

PlayerCameraScript::~PlayerCameraScript()
{
}

void PlayerCameraScript::LateUpdate()
{
	Vec3 pos = GetTransform()->GetLocalPosition();

	if (INPUT->GetButton(KEY_TYPE::W))
		pos += GetTransform()->GetLook() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::S))
		pos -= GetTransform()->GetLook() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::A))
		pos -= GetTransform()->GetRight() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::D))
		pos += GetTransform()->GetRight() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::Q))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x += DELTA_TIME * 0.5f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButton(KEY_TYPE::E))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x -= DELTA_TIME * 0.5f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButton(KEY_TYPE::Z))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y += DELTA_TIME * 0.5f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButton(KEY_TYPE::C))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y -= DELTA_TIME * 0.5f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON))
	{
		cout << "Camera Position : " << pos.x << ", " << pos.y << ", " << pos.z << endl;
		cout << "Camera Rotation : " << XMConvertToDegrees(GetTransform()->GetLocalRotation().x) << ", " << XMConvertToDegrees(GetTransform()->GetLocalRotation().y) << ", " << XMConvertToDegrees(GetTransform()->GetLocalRotation().z) << endl;
	}

	int g_myid = 1;	// 변경 필요
	shared_ptr<GameObject> playerObject = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjectByName(L"Hamster" + to_wstring(g_myid));

	// 플레이어의 위치를 가져온다.
	Vec3 playerPos = playerObject->GetTransform()->GetLocalPosition();

	// 카메라의 회전 설정
	float rotationX = XMConvertToRadians(_RotationX);
	float rotationY = XMConvertToRadians(_RotationY);
	Vec3 cameraRotation(rotationX, rotationY, 0.0f);
	GetTransform()->SetLocalRotation(cameraRotation);

	// 카메라의 위치 설정
	Vec3 cameraDirection = GetTransform()->GetLook();
	Vec3 cameraPos = playerPos - cameraDirection * _Distance + Vec3(0.0f, _Height, 0.0f);
	GetTransform()->SetLocalPosition(cameraPos);

	//GetTransform()->SetLocalPosition(pos);
}