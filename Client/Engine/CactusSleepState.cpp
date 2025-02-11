#include "pch.h"
#include "CactusSleepState.h"
#include "Cactus.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Camera.h"
#include "PlayerCameraScript.h"

void CactusSleepState::Enter(Cactus* cactus)
{
    cactus->PlayAnimation(CACTUS_STATE::SLEEP);
    m_detectionTimer = 0.0f;
    m_isPlayerDetected = false;
    ResetWakeUpDelay();
}

void CactusSleepState::Update(Cactus* cactus, float deltaTime)
{
    if (!m_isPlayerDetected) {
        // �÷��̾ Stage3 ������ ���Դ��� üũ
        if (IsPlayerInStage(cactus)) {
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
            if (IsPlayerInStage(cactus)) {
                cactus->SetState(CACTUS_STATE::AWAKE);
                Logger::Instance().Debug("��� �ð� ����. �������� ���");
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

void CactusSleepState::Exit(Cactus* cactus)
{
    m_detectionTimer = 0.0f;
    m_isPlayerDetected = false;
}

bool CactusSleepState::IsPlayerInStage(Cactus* cactus) const
{
    // Stage3 ���� ������ PlayerCameraScript���� �����ɴϴ�
    auto scene = GET_SINGLE(SceneManager)->GetActiveScene();
    auto mainCamera = scene->GetMainCamera();
    if (auto cameraScript = mainCamera->GetGameObject()->GetMonoBehaviour<PlayerCameraScript>()) {
		return cameraScript->IsPlayerInArea(L"Stage3");
    }

    return false;
}

void CactusSleepState::ResetWakeUpDelay() 
{
    // 0.5~2�� ������ ������ ��� �ð� ����
    float randomRatio = static_cast<float>(rand()) / RAND_MAX;
	m_wakeUpDelay = 0.5f + randomRatio * 1.5f;
}
