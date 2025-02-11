#pragma once
#include "MonoBehaviour.h"

enum class CameraMode {
    AREA_BASED,      // 영역 기반 고정 카메라
    PLAYER_CENTERED, // 플레이어 추적 카메라
    TRANSITIONING    // 전환 중
};

struct StageArea {
    wstring areaName;    // 영역 이름 (Stage1, Stage2, ...)
    Vec3 center;         // 영역 중심점
    Vec3 bounds;         // 영역 크기 (가로, 세로, 높이는 0)

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
    const StageArea* GetStageArea(const wstring& areaName) const;
    bool IsPlayerInArea(const wstring& areaName) const;

private:
    // 카메라 각도를 상수로 정의
    const float CAMERA_ROTATION_X = 70.f;
    const float CAMERA_ROTATION_Y = -180.f;
	
    float _Distance	= 1600.f;
    float _lerpSpeed = 1.5f;             // 보간 속도

private:
    // 카메라 모드
    CameraMode _currentMode = CameraMode::PLAYER_CENTERED;

    // 현재 영역 정보
    vector<StageArea> _cameraAreas;
    const StageArea* _currentArea = nullptr;

    // 이전 영역 정보 및 전환 관련 변수
    const StageArea* _previousArea = nullptr;
    float _transitionProgress = 0.0f;
    float _transitionDuration = 1.0f; // 전환 시간 (초)
    Vec3 _transitionStartPos;
    Vec3 _transitionEndPos;

    // 카메라 설정
    void InitializeCameraAreas();
    void UpdateCamera();
    void UpdateAreaBasedCamera();
    void UpdatePlayerCenteredCamera();
    void UpdateTransitioningCamera();
};

