#include "pch.h"
#include "BossSleepState.h"
#include "Boss.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Camera.h"
#include "PlayerCameraScript.h"

void BossSleepState::Enter(Boss* boss) {
    boss->PlayAnimation(BOSS_STATE::SLEEP);
    Logger::Instance().Debug("보스가 Sleep 상태로 진입");
}

void BossSleepState::Update(Boss* boss, float deltaTime) {
    // Stage9 영역에 플레이어가 있는지 체크
    if (IsPlayerInStage(boss)) {
        boss->SetState(BOSS_STATE::AWAKE);
        Logger::Instance().Debug("플레이어 감지, Awake 상태로 전환");
    }
}

void BossSleepState::Exit(Boss* boss)
{
}

bool BossSleepState::IsPlayerInStage(Boss* boss) const {
    auto scene = GET_SINGLE(SceneManager)->GetActiveScene();
    auto mainCamera = scene->GetMainCamera();
    if (auto cameraScript = mainCamera->GetGameObject()->GetMonoBehaviour<PlayerCameraScript>()) {
        return cameraScript->IsPlayerInArea(L"Stage9");
    }
    return false;
}