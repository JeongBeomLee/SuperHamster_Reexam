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
        // �÷��̾ Stage5 ������ ���Դ��� üũ
        if (IsPlayerInStage(chestMonster)) {
            m_isPlayerDetected = true;
            ResetWakeUpDelay();
            Logger::Instance().Debug("�÷��̾� ����. {}�� �� ���", m_wakeUpDelay);
        }
    }
    else {
        // �÷��̾ ������ �� ���������� ��� �ð� ���
        m_detectionTimer += deltaTime;

        if (m_detectionTimer >= m_wakeUpDelay) {
            // ��� �ð��� ������ ���
            if (IsPlayerInStage(chestMonster)) {
                chestMonster->SetState(CHEST_MONSTER_STATE::AWAKE);
                Logger::Instance().Debug("��� �ð� ����. �̹��� ���");
            }
            else {
                // ��� �ð� ���� �÷��̾ ������ ��� ���
                m_isPlayerDetected = false;
                m_detectionTimer = 0.0f;
                Logger::Instance().Debug("�÷��̾ ������ ���. ��� ���� ����");
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
    // Stage3 ���� ������ PlayerCameraScript���� �����ɴϴ�
    auto scene = GET_SINGLE(SceneManager)->GetActiveScene();
    auto mainCamera = scene->GetMainCamera();
    if (auto cameraScript = mainCamera->GetGameObject()->GetMonoBehaviour<PlayerCameraScript>()) {
        return cameraScript->IsPlayerInArea(L"Stage5");
    }

    return false;
}

void ChestMonsterSleepState::ResetWakeUpDelay()
{
    // 0.5~2�� ������ ������ ��� �ð� ����
    float randomRatio = static_cast<float>(rand()) / RAND_MAX;
    m_wakeUpDelay = 0.5f + randomRatio * 1.5f;
}
