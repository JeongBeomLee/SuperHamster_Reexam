#include "pch.h"
#include "ChestMonsterSleepState.h"
#include "ChestMonster.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Camera.h"
#include "PlayerCameraScript.h"

void ChestMonsterSleepState::Enter(ChestMonster* chestMonster)
{
    chestMonster->PlayAnimation(CHEST_MONSTER_STATE::SLEEP);
    m_detectionTimer = 0.0f;
    m_isPlayerDetected = false;
    ResetWakeUpDelay();
}

void ChestMonsterSleepState::Update(ChestMonster* chestMonster, float deltaTime)
{
    if (!m_isPlayerDetected) {
        // 플레이어가 Stage5 영역에 들어왔는지 체크
        if (IsPlayerInStage(chestMonster)) {
            m_isPlayerDetected = true;
            ResetWakeUpDelay();
            Logger::Instance().Debug("플레이어 감지. {}초 후 깨어남", m_wakeUpDelay);
        }
    }
    else {
        // 플레이어가 감지된 후 깨어나기까지의 대기 시간 계산
        m_detectionTimer += deltaTime;

        if (m_detectionTimer >= m_wakeUpDelay) {
            // 대기 시간이 지나면 깨어남
            if (IsPlayerInStage(chestMonster)) {
                chestMonster->SetState(CHEST_MONSTER_STATE::AWAKE);
                Logger::Instance().Debug("대기 시간 종료. 미믹이 깨어남");
            }
            else {
                // 대기 시간 동안 플레이어가 영역을 벗어난 경우
                m_isPlayerDetected = false;
                m_detectionTimer = 0.0f;
                Logger::Instance().Debug("플레이어가 영역을 벗어남. 대기 상태 리셋");
            }
        }
    }
}

void ChestMonsterSleepState::Exit(ChestMonster* chestMonster)
{
    m_detectionTimer = 0.0f;
    m_isPlayerDetected = false;
}

bool ChestMonsterSleepState::IsPlayerInStage(ChestMonster* chestMonster) const
{
    // Stage3 영역 정보는 PlayerCameraScript에서 가져옵니다
    auto scene = GET_SINGLE(SceneManager)->GetActiveScene();
    auto mainCamera = scene->GetMainCamera();
    if (auto cameraScript = mainCamera->GetGameObject()->GetMonoBehaviour<PlayerCameraScript>()) {
        return cameraScript->IsPlayerInArea(L"Stage5");
    }

    return false;
}

void ChestMonsterSleepState::ResetWakeUpDelay()
{
    // 0.5~2초 사이의 랜덤한 대기 시간 설정
    float randomRatio = static_cast<float>(rand()) / RAND_MAX;
    m_wakeUpDelay = 0.5f + randomRatio * 1.5f;
}
