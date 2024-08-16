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
            // ���� ��ġ�� �ε巴�� ����
            Vec3 currentPos = GetTransform()->GetLocalPosition();
            Vec3 newPos = Vec3::Lerp(currentPos, _serverPosition, _lerpSpeed * DELTA_TIME);
            player->SetPosition(newPos);

            // ���� ������Ʈ
            if (_serverDirection != Vec3::Zero) {
                Vec3 correctedDir = Vec3(-_serverDirection.x, 0, -_serverDirection.z);

                // ���� ȸ���� ��ǥ ȸ�� ���̸� ����
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

    // SPACE Ű�� ������ �� ���� ����
    if (INPUT->GetButtonDown(KEY_TYPE::SPACE))
    {
        StartRoll();
        return;
    }

	if (player->GetCurrentState() == PLAYER_STATE::ROLL) return;

    // A Ű�� ������ �ִ� ���� AIM ���� ����
    if (INPUT->GetButton(KEY_TYPE::A))
    {
        player->SetState(PLAYER_STATE::AIM);
    }

    Vec3 moveDir = Vec3::Zero;

    // ī�޶� �������� �̵� ���� ����
    auto camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
    Vec3 cameraForward = camera->GetTransform()->GetLook();
    Vec3 cameraRight = camera->GetTransform()->GetRight();

    // ī�޶��� up vector�� �����ϰ� XZ ��鿡 ����
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
            // ���ÿ��� ��� �̵�
            Vec3 newPos = GetTransform()->GetLocalPosition() + moveDir * _moveSpeed * DELTA_TIME;
            player->SetPosition(newPos);
        }

        // ������ �̵� ��Ŷ ����
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

        // ���� �̵� ���� �Ǵ� �ٶ󺸴� �������� ����
        _rollDirection = _serverDirection != Vec3::Zero ? _serverDirection : GetTransform()->GetLook();
        _rollDirection.y = 0; // Y�� �̵� ����
        _rollDirection.Normalize();

        // ������ ���� ���� �˸�
        GEngine->GetNetworkManager()->SendStateUpdate(_playerId, PLAYER_STATE::ROLL);
    }
}

void PlayerMove::UpdateRoll(float deltaTime)
{
    _rollTimer += deltaTime;
    if (_rollTimer >= _rollDuration)
    {
        // ���� ����
        Player* player = GET_SINGLE(PlayerManager)->GetPlayer(_playerId);
        if (player)
        {
            player->SetState(PLAYER_STATE::IDLE);
            GEngine->GetNetworkManager()->SendStateUpdate(_playerId, PLAYER_STATE::IDLE);
        }
        return;
    }
    
    // ���� �̵� ����
    Vec3 rollMovement = _rollDirection * _rollSpeed * deltaTime;
    Vec3 newPos = GetTransform()->GetLocalPosition() + rollMovement;
    GetTransform()->SetLocalPosition(newPos);

    // ������ �̵� ��Ŷ ����
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
