#pragma once
#include "MonoBehaviour.h"
#include "PlayerStateMachine.h"
#include "Engine.h"

class PlayerMove : public MonoBehaviour
{
public:
    PlayerMove();
    PlayerMove(uint32_t playerId);
    virtual ~PlayerMove();

    virtual void Start() override;
    virtual void Update() override;

    void ProcessInput();

    void StartRoll();
    void UpdateRoll(float deltaTime);

    float LerpAngle(float a, float b, float t);
    float repeat(float t, float length);

    void ChangeState(PLAYER_STATE newState);
    PLAYER_STATE GetCurrentState() const;

    bool IsLocal() const { return _playerId == GEngine->GetMyPlayerId(); }

private:
    uint32_t _playerId = -1;
    float _moveSpeed = 500.0f;
    Vec3 _inputDirection = Vec3::Zero;

    float _rollSpeed = 1000.0f;  // ���� �ӵ�
    float _rollDuration = 0.5f; // ���� ���� �ð�
    float _rollTimer = 0.0f;    // ���� Ÿ�̸�
    Vec3 _rollDirection = Vec3::Zero; // ���� ����
};
