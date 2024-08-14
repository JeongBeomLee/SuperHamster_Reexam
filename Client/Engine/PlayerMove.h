#pragma once
#include "MonoBehaviour.h"

class PlayerMove : public MonoBehaviour
{
public:
    PlayerMove();
    virtual ~PlayerMove();

    virtual void Start() override;
    virtual void Update() override;

    void ProcessInput();
    void SendMovePacket(const Vec3& direction);
    void HandleMoveResult(const S2C_MoveResultPacket& packet);

    float LerpAngle(float a, float b, float t);
    float repeat(float t, float length);

private:
    float _moveSpeed = 50.0f;
    uint32_t _playerId;
    Vec3 _serverPosition;
    Vec3 _serverDirection;
    float _lerpSpeed = 10.0f; // 서버 위치로의 보간 속도
    float _rotationLerpSpeed = 10.0f; // 회전 보간 속도
};
