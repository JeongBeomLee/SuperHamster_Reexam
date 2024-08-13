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
	int g_myid = 1;	// 서버에서 받아온 ID로 변경 필요
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
}