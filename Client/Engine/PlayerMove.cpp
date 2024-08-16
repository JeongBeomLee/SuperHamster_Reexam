#include "pch.h"
#include "PlayerMove.h"
#include "Input.h"
#include "NetworkManager.h"
#include "Transform.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "Timer.h"
#include "PlayerManager.h"

PlayerMove::PlayerMove()
{
}

PlayerMove::PlayerMove(uint32_t playerId)
{
    _playerId = playerId;
}

PlayerMove::~PlayerMove()
{
}

void PlayerMove::Update()
{
    if (IsLocal())
    {
        ProcessInput();
    }
    Player* player = GET_SINGLE(PlayerManager)->GetPlayer(_playerId);
    if (player)
    {
        player->Update(DELTA_TIME);

        if (GetCurrentState() == PLAYER_STATE::ROLL)
        {
            UpdateRoll(DELTA_TIME);
        }
        else
        {
            // 서버 위치로 부드럽게 보간
            Vec3 currentPos = GetTransform()->GetLocalPosition();
            Vec3 newPos = Vec3::Lerp(currentPos, _serverPosition, _lerpSpeed * DELTA_TIME);
            player->SetPosition(newPos);

            // 방향 업데이트
            if (_serverDirection != Vec3::Zero) {
                Vec3 correctedDir = Vec3(-_serverDirection.x, 0, -_serverDirection.z);

                // 현재 회전과 목표 회전 사이를 보간
                Vec3 currentRotation = GetTransform()->GetLocalRotation();
                float targetYaw = atan2(correctedDir.x, correctedDir.z);
                float newYaw = LerpAngle(currentRotation.y, targetYaw, _rotationLerpSpeed * DELTA_TIME);

                player->SetRotation(Vec3(-XM_PIDIV2, newYaw, 0));
            }
        }
    }
}

void PlayerMove::Start()
{
    _serverPosition = GetTransform()->GetLocalPosition();
    _serverDirection = GetTransform()->GetLocalRotation();
}

void PlayerMove::ProcessInput()
{
    Player* player = GET_SINGLE(PlayerManager)->GetPlayer(_playerId);

    // SPACE 키를 눌렀을 때 돌진 시작
    if (INPUT->GetButtonDown(KEY_TYPE::SPACE))
    {
        StartRoll();
        return;
    }

	if (player->GetCurrentState() == PLAYER_STATE::ROLL) return;

    // A 키를 누르고 있는 동안 AIM 상태 유지
    if (INPUT->GetButton(KEY_TYPE::A))
    {
        player->SetState(PLAYER_STATE::AIM);
    }

    Vec3 moveDir = Vec3::Zero;

    // 카메라 기준으로 이동 방향 결정
    auto camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
    Vec3 cameraForward = camera->GetTransform()->GetLook();
    Vec3 cameraRight = camera->GetTransform()->GetRight();

    // 카메라의 up vector를 무시하고 XZ 평면에 투영
    cameraForward.y = 0;
    cameraRight.y = 0;
    cameraForward.Normalize();
    cameraRight.Normalize();

    if (INPUT->GetButton(KEY_TYPE::UP))
        moveDir += cameraForward;
    if (INPUT->GetButton(KEY_TYPE::DOWN))
        moveDir -= cameraForward;
    if (INPUT->GetButton(KEY_TYPE::LEFT))
        moveDir -= cameraRight;
    if (INPUT->GetButton(KEY_TYPE::RIGHT))
        moveDir += cameraRight;

    if (moveDir != Vec3::Zero)
    {
        moveDir.Normalize();

        if (GetCurrentState() != PLAYER_STATE::AIM)
        {
            player->SetState(PLAYER_STATE::RUN);
            // 로컬에서 즉시 이동
            Vec3 newPos = GetTransform()->GetLocalPosition() + moveDir * _moveSpeed * DELTA_TIME;
            player->SetPosition(newPos);
        }

        // 서버에 이동 패킷 전송
        SendMovePacket(moveDir);
    }
    else if (GetCurrentState() != PLAYER_STATE::AIM)
    {
        player->SetState(PLAYER_STATE::IDLE);
    }
}

void PlayerMove::SendMovePacket(const Vec3& direction)
{
    C2S_MovePacket* packet = new C2S_MovePacket();
    packet->type = PacketType::C2S_MOVE;
    packet->size = sizeof(C2S_MovePacket);
    packet->playerId = _playerId;
    packet->dirX = direction.x;
    packet->dirZ = direction.z;

    GEngine->GetNetworkManager()->SendPacket(packet);
}

void PlayerMove::HandleMoveResult(const S2C_MoveResultPacket& packet)
{
    if (packet.playerId == _playerId)
    {
        _serverPosition = Vec3(packet.posX, packet.posY, packet.posZ);
        _serverDirection = Vec3(packet.dirX, packet.dirY, packet.dirZ);
    }
}

void PlayerMove::StartRoll()
{
    Player* player = GET_SINGLE(PlayerManager)->GetPlayer(_playerId);
    if (player)
    {
        player->SetState(PLAYER_STATE::ROLL);
        _rollTimer = 0.0f;

        // 현재 이동 방향 또는 바라보는 방향으로 돌진
        _rollDirection = _serverDirection != Vec3::Zero ? _serverDirection : GetTransform()->GetLook();
        _rollDirection.y = 0; // Y축 이동 방지
        _rollDirection.Normalize();

        // 서버에 상태 변경 알림
        GEngine->GetNetworkManager()->SendStateUpdate(_playerId, PLAYER_STATE::ROLL);
    }
}

void PlayerMove::UpdateRoll(float deltaTime)
{
    _rollTimer += deltaTime;
    if (_rollTimer >= _rollDuration)
    {
        // 돌진 종료
        Player* player = GET_SINGLE(PlayerManager)->GetPlayer(_playerId);
        if (player)
        {
            player->SetState(PLAYER_STATE::IDLE);
            GEngine->GetNetworkManager()->SendStateUpdate(_playerId, PLAYER_STATE::IDLE);
        }
        return;
    }
    
    // 돌진 이동 적용
    Vec3 rollMovement = _rollDirection * _rollSpeed * deltaTime;
    Vec3 newPos = GetTransform()->GetLocalPosition() + rollMovement;
    GetTransform()->SetLocalPosition(newPos);

    // 서버에 이동 패킷 전송
    SendMovePacket(_rollDirection);
}

float PlayerMove::LerpAngle(float a, float b, float t)
{
    float delta = repeat((b - a), XM_2PI);
    if (delta > XM_PI)
        delta -= XM_2PI;
    return a + delta * t;
}

float PlayerMove::repeat(float t, float length)
{
    return t - floor(t / length) * length;
}

void PlayerMove::ChangeState(PLAYER_STATE newState)
{
    Player* player = GET_SINGLE(PlayerManager)->GetPlayer(_playerId);
    if (player)
    {
        player->SetState(newState);
    }
}

PLAYER_STATE PlayerMove::GetCurrentState() const
{
    Player* player = GET_SINGLE(PlayerManager)->GetPlayer(_playerId);
    return player ? player->GetCurrentState() : PLAYER_STATE::END;
}
