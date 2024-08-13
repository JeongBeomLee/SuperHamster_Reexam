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
	int g_myid = 1;	// �������� �޾ƿ� ID�� ���� �ʿ�
	shared_ptr<GameObject> playerObject = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjectByName(L"Hamster" + to_wstring(g_myid));

	// �÷��̾��� ��ġ�� �����´�.
	Vec3 playerPos = playerObject->GetTransform()->GetLocalPosition();

	// ī�޶��� ȸ�� ����
	float rotationX = XMConvertToRadians(_RotationX);
	float rotationY = XMConvertToRadians(_RotationY);
	Vec3 cameraRotation(rotationX, rotationY, 0.0f);
	GetTransform()->SetLocalRotation(cameraRotation);

	// ī�޶��� ��ġ ����
	Vec3 cameraDirection = GetTransform()->GetLook();
	Vec3 cameraPos = playerPos - cameraDirection * _Distance + Vec3(0.0f, _Height, 0.0f);
	GetTransform()->SetLocalPosition(cameraPos);
}