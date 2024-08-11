#pragma once
#include <cstdint>

// 패킷 최대 크기
constexpr int MAX_PACKET_SIZE = 1024;
constexpr char SERVER_IP[] = "127.0.0.1";
//constexpr char SERVER_IP[] = "192.168.0.15";
constexpr int PORT = 9000;

// 패킷 타입 정의
enum class PacketType : uint16_t
{
    C2S_LOGIN = 1,           // 클라이언트 로그인 요청
    S2C_LOGIN_RESULT,        // 서버 로그인 응답
    C2S_GAME_START,          // 게임 시작 요청
    S2C_GAME_START_RESULT,   // 게임 시작 응답
    C2S_MOVE,                // 이동 요청
    S2C_MOVE_RESULT,         // 이동 결과
    C2S_ATTACK,              // 공격 요청
    S2C_ATTACK_RESULT,       // 공격 결과
    S2C_OBJECT_SPAWN,        // 오브젝트 스폰
    S2C_OBJECT_DESPAWN,      // 오브젝트 제거
    S2C_SYNC_TRANSFORM,      // 트랜스폼 동기화
    C2S_INTERACTION,         // 상호작용 요청
    S2C_INTERACTION_RESULT,  // 상호작용 결과
    S2C_GAME_OVER,           // 게임 오버
};

// 패킷 헤더
#pragma pack(push, 1)
struct PacketHeader
{
    uint16_t size;
    PacketType type;
};
#pragma pack(pop)

// 로그인 요청 패킷
struct C2S_LoginPacket : PacketHeader
{
};

// 로그인 응답 패킷
struct S2C_LoginResultPacket : PacketHeader
{
    bool success;
    uint32_t playerId;
};

// 게임 시작 요청 패킷
struct C2S_GameStartPacket : PacketHeader
{
    uint32_t playerId;
};

// 게임 시작 응답 패킷
struct S2C_GameStartResultPacket : PacketHeader
{
    bool success;
    uint32_t playerNumber; // 1 or 2
};

// 이동 요청 패킷
struct C2S_MovePacket : PacketHeader
{
    uint32_t playerId;
    float posX;
    float posY;
    float posZ;
    float dirX;
    float dirY;
    float dirZ;
};

// 이동 결과 패킷
struct S2C_MoveResultPacket : PacketHeader
{
    uint32_t playerId;
    float posX;
    float posY;
    float posZ;
    float dirX;
    float dirY;
    float dirZ;
};

// 공격 요청 패킷
struct C2S_AttackPacket : PacketHeader
{
    uint32_t playerId;
    uint32_t attackType;
};

// 공격 결과 패킷
struct S2C_AttackResultPacket : PacketHeader
{
    uint32_t playerId;
    uint32_t attackType;
    bool hit;
    uint32_t targetId;
    int32_t damage;
};

// 오브젝트 스폰 패킷
struct S2C_ObjectSpawnPacket : PacketHeader
{
    uint32_t objectId;
    uint32_t objectType;
    float posX;
    float posY;
    float posZ;
};

// 오브젝트 제거 패킷
struct S2C_ObjectDespawnPacket : PacketHeader
{
    uint32_t objectId;
};

// 트랜스폼 동기화 패킷
struct S2C_SyncTransformPacket : PacketHeader
{
    uint32_t objectId;
    float posX;
    float posY;
    float posZ;
    float rotX;
    float rotY;
    float rotZ;
    float rotW;
};

// 상호작용 요청 패킷
struct C2S_InteractionPacket : PacketHeader
{
    uint32_t playerId;
    uint32_t targetId;
    uint32_t interactionType;
};

// 상호작용 결과 패킷
struct S2C_InteractionResultPacket : PacketHeader
{
    uint32_t playerId;
    uint32_t targetId;
    uint32_t interactionType;
    bool success;
};

// 게임 오버 패킷
struct S2C_GameOverPacket : PacketHeader
{
    bool victory;
    uint32_t score;
};