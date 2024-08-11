#pragma once
#include <cstdint>

// ��Ŷ �ִ� ũ��
constexpr int MAX_PACKET_SIZE = 1024;
constexpr char SERVER_IP[] = "127.0.0.1";
//constexpr char SERVER_IP[] = "192.168.0.15";
constexpr int PORT = 9000;

// ��Ŷ Ÿ�� ����
enum class PacketType : uint16_t
{
    C2S_LOGIN = 1,           // Ŭ���̾�Ʈ �α��� ��û
    S2C_LOGIN_RESULT,        // ���� �α��� ����
    C2S_GAME_START,          // ���� ���� ��û
    S2C_GAME_START_RESULT,   // ���� ���� ����
    C2S_MOVE,                // �̵� ��û
    S2C_MOVE_RESULT,         // �̵� ���
    C2S_ATTACK,              // ���� ��û
    S2C_ATTACK_RESULT,       // ���� ���
    S2C_OBJECT_SPAWN,        // ������Ʈ ����
    S2C_OBJECT_DESPAWN,      // ������Ʈ ����
    S2C_SYNC_TRANSFORM,      // Ʈ������ ����ȭ
    C2S_INTERACTION,         // ��ȣ�ۿ� ��û
    S2C_INTERACTION_RESULT,  // ��ȣ�ۿ� ���
    S2C_GAME_OVER,           // ���� ����
};

// ��Ŷ ���
#pragma pack(push, 1)
struct PacketHeader
{
    uint16_t size;
    PacketType type;
};
#pragma pack(pop)

// �α��� ��û ��Ŷ
struct C2S_LoginPacket : PacketHeader
{
};

// �α��� ���� ��Ŷ
struct S2C_LoginResultPacket : PacketHeader
{
    bool success;
    uint32_t playerId;
};

// ���� ���� ��û ��Ŷ
struct C2S_GameStartPacket : PacketHeader
{
    uint32_t playerId;
};

// ���� ���� ���� ��Ŷ
struct S2C_GameStartResultPacket : PacketHeader
{
    bool success;
    uint32_t playerNumber; // 1 or 2
};

// �̵� ��û ��Ŷ
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

// �̵� ��� ��Ŷ
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

// ���� ��û ��Ŷ
struct C2S_AttackPacket : PacketHeader
{
    uint32_t playerId;
    uint32_t attackType;
};

// ���� ��� ��Ŷ
struct S2C_AttackResultPacket : PacketHeader
{
    uint32_t playerId;
    uint32_t attackType;
    bool hit;
    uint32_t targetId;
    int32_t damage;
};

// ������Ʈ ���� ��Ŷ
struct S2C_ObjectSpawnPacket : PacketHeader
{
    uint32_t objectId;
    uint32_t objectType;
    float posX;
    float posY;
    float posZ;
};

// ������Ʈ ���� ��Ŷ
struct S2C_ObjectDespawnPacket : PacketHeader
{
    uint32_t objectId;
};

// Ʈ������ ����ȭ ��Ŷ
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

// ��ȣ�ۿ� ��û ��Ŷ
struct C2S_InteractionPacket : PacketHeader
{
    uint32_t playerId;
    uint32_t targetId;
    uint32_t interactionType;
};

// ��ȣ�ۿ� ��� ��Ŷ
struct S2C_InteractionResultPacket : PacketHeader
{
    uint32_t playerId;
    uint32_t targetId;
    uint32_t interactionType;
    bool success;
};

// ���� ���� ��Ŷ
struct S2C_GameOverPacket : PacketHeader
{
    bool victory;
    uint32_t score;
};