#include "pch.h"
#include "PlayerCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Engine.h"
#include "Player.h"
#include "PlayerManager.h"

PlayerCameraScript::PlayerCameraScript()
{
	// ī�޶� ���� �ʱ�ȭ
	InitializeCameraAreas();
}

PlayerCameraScript::~PlayerCameraScript()
{
}

void PlayerCameraScript::LateUpdate()
{
	int myPlayerId = GEngine->GetMyPlayerId();
	auto player = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjectByName(L"Player" + to_wstring(myPlayerId));
	if (!player)
		return;

	Vec3 playerPos = player->GetTransform()->GetLocalPosition();

	// ī�޶��� ȸ�� ���� (������ ����)
	float rotationX = XMConvertToRadians(CAMERA_ROTATION_X);
	float rotationY = XMConvertToRadians(CAMERA_ROTATION_Y);
	Vec3 cameraRotation(rotationX, rotationY, 0.0f);
	GetTransform()->SetLocalRotation(cameraRotation);

	// ���� ���� ����
	const StageArea* previousArea = _currentArea;

	// ���� ���� üũ
	bool foundArea = false;
	for (const auto& area : _cameraAreas) {
		if (area.Contains(playerPos)) {
			_currentArea = &area;
			foundArea = true;
			break;
		}
	}

	if (!foundArea) {
		_currentArea = nullptr;
	}

	// ���� ���� ����
	if (_currentArea != previousArea) {
		// ��ȯ ����
		_previousArea = previousArea;
		_transitionProgress = 0.0f;
		_transitionStartPos = GetTransform()->GetLocalPosition();

		// ���ο� ī�޶� ��ġ ���
		Vec3 cameraDirection = GetTransform()->GetLook();
		if (_currentArea) {
			_transitionEndPos = _currentArea->center - (cameraDirection * _Distance);
		}
		else {
			_transitionEndPos = playerPos - (cameraDirection * _Distance);
		}

		_currentMode = CameraMode::TRANSITIONING;
	}

	UpdateCamera();
}

const StageArea* PlayerCameraScript::GetStageArea(const wstring& areaName) const
{
	for (const auto& area : _cameraAreas) {
		if (area.areaName == areaName) {
			return &area;
		}
	}
	return nullptr;
}

bool PlayerCameraScript::IsPlayerInArea(const wstring& areaName) const
{
	const StageArea* area = GetStageArea(areaName);
	if (!area) return false;

	auto scene = GET_SINGLE(SceneManager)->GetActiveScene();
	// ��� �÷��̾ ���� �˻�
	const auto& players = GET_SINGLE(PlayerManager)->GetPlayers();
	for (const auto& [playerId, player] : players) {
		if (!player || !player->GetGameObject()) continue;

		Vec3 playerPos = player->GetGameObject()->GetTransform()->GetLocalPosition();
		if (area->Contains(playerPos)) {
			return true;  // �� ���̶� ���� �ȿ� ������ true ��ȯ
		}
	}

	return false;
}

void PlayerCameraScript::InitializeCameraAreas()
{
	_cameraAreas.push_back({
		L"Stage1",
		Vec3(0.0f, 110.f, 0.0f),     // center
		Vec3(1200.f, 0.f, 1200.f),               // bounds
		});

	_cameraAreas.push_back({
		L"Stage2",
		Vec3(0.0f, 110.f, -1700.0f),     // center
		Vec3(1200.f, 0.f, 1200.f),               // bounds
		});

	_cameraAreas.push_back({
		L"Stage3",
		Vec3(1944.9496f, 110.f, -1696.3253f),     // center
		Vec3(1200.f, 0.f, 1200.f),               // bounds
		});

	_cameraAreas.push_back({
		L"Stage4",
		Vec3(1955.0245f, 110.f, -6.5450735f),     // center
		Vec3(1200.f, 0.f, 1200.f),               // bounds
		});

	_cameraAreas.push_back({
		L"Stage5",
		Vec3(1944.4672f, 110.f, -3457.5078f),     // center
		Vec3(1200.f, 0.f, 1200.f),               // bounds
		});

	_cameraAreas.push_back({
		L"Stage6",
		Vec3(-1956.9929f, 110.f, -1798.4175f),     // center
		Vec3(1200.f, 0.f, 1200.f),               // bounds
		});

	_cameraAreas.push_back({
		L"Stage7",
		Vec3(-2049.9458f, 110.f, -106.035675f),     // center
		Vec3(1200.f, 0.f, 1200.f),               // bounds
		});

	_cameraAreas.push_back({
		L"Stage8",
		Vec3(-2003.9424f, 110.f, 1490.736f),     // center
		Vec3(1200.f, 0.f, 1200.f),               // bounds
		});

	_cameraAreas.push_back({
		L"Stage9",
		Vec3(-8.31838f, 110.f, -3500.4863f),     // center
		Vec3(1500.f, 0.f, 1400.f),               // bounds
		});
}

void PlayerCameraScript::UpdateCamera()
{
	switch (_currentMode) {
	case CameraMode::AREA_BASED:
		UpdateAreaBasedCamera();
		break;
	case CameraMode::PLAYER_CENTERED:
		UpdatePlayerCenteredCamera();
		break;
	case CameraMode::TRANSITIONING:
		UpdateTransitioningCamera();
		break;
	}
}

void PlayerCameraScript::UpdateAreaBasedCamera()
{
	if (!_currentArea)
		return;

	// ī�޶��� ��ġ ����
	Vec3 cameraDirection = GetTransform()->GetLook();
	Vec3 cameraPos = _currentArea->center - (cameraDirection * _Distance);

	GetTransform()->SetLocalPosition(cameraPos);
}

void PlayerCameraScript::UpdatePlayerCenteredCamera()
{
	int myPlayerId = GEngine->GetMyPlayerId();
	auto player = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjectByName(L"Player" + to_wstring(myPlayerId));
	if (!player)
		return;

	Vec3 playerPos = player->GetTransform()->GetLocalPosition();

	// ī�޶��� ��ġ ����
	Vec3 cameraDirection = GetTransform()->GetLook();
	Vec3 cameraPos = playerPos - (cameraDirection * _Distance);

	GetTransform()->SetLocalPosition(cameraPos);
}

void PlayerCameraScript::UpdateTransitioningCamera()
{
	// �ð��� ���� ���� ����
	_transitionProgress += GET_SINGLE(Timer)->GetDeltaTime() / (_transitionDuration / _lerpSpeed);
	if (_transitionProgress >= 1.0f) {
		_transitionProgress = 1.0f;

		// ��ȯ �Ϸ� �� ��� ����
		if (_currentArea) {
			_currentMode = CameraMode::AREA_BASED;
		}
		else {
			_currentMode = CameraMode::PLAYER_CENTERED;
		}
	}

	// SmootherStep
	float t = _transitionProgress;
	t = t * t * (3.0f - 2.0f * t);

	// ������ ��ġ ���
	Vec3 interpolatedPos = Vec3::Lerp(_transitionStartPos, _transitionEndPos, t);
	GetTransform()->SetLocalPosition(interpolatedPos);
}
