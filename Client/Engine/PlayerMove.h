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
    void SendMovePacket(const Vec3& direction);
    void HandleMoveResult(const S2C_MoveResultPacket& packet);

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
    Vec3 _serverPosition;
    Vec3 _serverDirection;
    float _lerpSpeed = 10.0f; // 서버 위치로의 보간 속도
    float _rotationLerpSpeed = 10.0f; // 회전 보간 속도

    float _rollSpeed = 1000.0f;  // 돌진 속도
    float _rollDuration = 0.5f; // 돌진 지속 시간
    float _rollTimer = 0.0f;    // 돌진 타이머
    Vec3 _rollDirection = Vec3::Zero; // 돌진 방향
};
