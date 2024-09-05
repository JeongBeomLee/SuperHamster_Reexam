#include "pch.h"
#include "PlayerMove.h"
#include "Input.h"
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
    }
}

void PlayerMove::Start()
{
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

    // ����Ű �Է¿� ���� �̵� ���� ����
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
    }
    else if (GetCurrentState() != PLAYER_STATE::AIM)
    {
        player->SetState(PLAYER_STATE::IDLE);
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
        _rollDirection = GetTransform()->GetLook();
        _rollDirection.y = 0; // Y�� �̵� ����
        _rollDirection.Normalize();
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
        }
        return;
    }
    
    // ���� �̵� ����
    Vec3 rollMovement = _rollDirection * _rollSpeed * deltaTime;
    Vec3 newPos = GetTransform()->GetLocalPosition() + rollMovement;
    GetTransform()->SetLocalPosition(newPos);
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
