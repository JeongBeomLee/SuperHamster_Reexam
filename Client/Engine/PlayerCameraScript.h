#pragma once
#include "MonoBehaviour.h"

enum class CameraMode {
    AREA_BASED,      // ���� ��� ���� ī�޶�
    PLAYER_CENTERED, // �÷��̾� ���� ī�޶�
    TRANSITIONING    // ��ȯ ��
};

struct CameraArea {
    wstring areaName;    // ���� �̸� (Stage1, Stage2, ...)
    Vec3 center;         // ���� �߽���
    Vec3 bounds;         // ���� ũ�� (����, ����, ����)

    bool Contains(const Vec3& point) const {
        Vec3 minBounds = center - (bounds * 0.5f);
        Vec3 maxBounds = center + (bounds * 0.5f);

        return point.x >= minBounds.x && point.x <= maxBounds.x
            && point.z >= minBounds.z && point.z <= maxBounds.z;
    }
};

class PlayerCameraScript : public MonoBehaviour
{
public:
	PlayerCameraScript();
	virtual ~PlayerCameraScript();

	virtual void LateUpdate() override;

	void SetDistance(float distance)	{ _Distance = distance; }

private:
    // ī�޶� ������ ����� ����
    const float CAMERA_ROTATION_X = 70.f;
    const float CAMERA_ROTATION_Y = -180.f;
	
    float _Distance	= 1600.f;`
    float _lerpSpeed = 1.5f;             // ���� �ӵ�

private:
    // ī�޶� ���
    CameraMode _currentMode = CameraMode::PLAYER_CENTERED;

    // ���� ���� ����
    vector<CameraArea> _cameraAreas;
    const CameraArea* _currentArea = nullptr;

    // ���� ���� ���� �� ��ȯ ���� ����
    const CameraArea* _previousArea = nullptr;
    float _transitionProgress = 0.0f;
    float _transitionDuration = 1.0f; // ��ȯ �ð� (��)
    Vec3 _transitionStartPos;
    Vec3 _transitionEndPos;

    // ī�޶� ����
    void InitializeCameraAreas();
    void UpdateCamera();
    void UpdateAreaBasedCamera();
    void UpdatePlayerCenteredCamera();
    void UpdateTransitioningCamera();
};

