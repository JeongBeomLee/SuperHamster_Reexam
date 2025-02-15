#pragma once
#include "PhysicsTypes.h"
#include "GameObject.h"
#include "PhysicsObject.h"

// �ڷ���Ʈ ������ ��� ����ü
struct TeleportZone {
    TeleportZoneId id;           // �ڷ���Ʈ �� ���� ID
    Vec3 sourcePosition;         // ��� ���� ��ġ
    Vec3 destinationPosition;    // ���� ���� ��ġ
    Vec3 triggerSize;            // Ʈ���� ũ��
    string sourceName;          // ��� ���� �̸�
    string destinationName;     // ���� ���� �̸�

    TeleportZone() : id(TeleportZoneId::NONE) {}
    TeleportZone(TeleportZoneId _id,
        const Vec3& source, const Vec3& dest,
        const Vec3& size,
        const string& srcName, const string& dstName)
        : id(_id)
        , sourcePosition(source)
        , destinationPosition(dest)
        , triggerSize(size)
        , sourceName(srcName)
        , destinationName(dstName)
    {
    }
};

class TeleportSystem
{
private:
	TeleportSystem() = default;
    ~TeleportSystem();

public:
    static TeleportSystem* GetInstance() {
        static TeleportSystem instance;
        return &instance;
    }

    void Initialize();
    void Update();

    // �ڷ���Ʈ �� ���� �� ����
    void CreateTeleportZones();
    const TeleportZone* GetTeleportZone(TeleportZoneId id) const;

    // �ڷ���Ʈ ����
    void TriggerTeleport(GameObject* player, TeleportZoneId id);

    // ������
    void DebugTeleportZones();

private:
    // �ڷ���Ʈ Ʈ���� ���� ��ƿ��Ƽ
    std::shared_ptr<GameObject> CreateTeleportTrigger(
        const Vec3& position,
        const Vec3& size,
        TeleportZoneId id);
    bool IsInCooldown(GameObject* player) const;
    void StartCooldown(GameObject* player);

private:
    const float TELEPORT_COOLDOWN = 1.0f;  // 1���� ��ٿ� �ð�
    std::unordered_map<TeleportZoneId, TeleportZone> m_teleportZones;
    std::unordered_map<GameObject*, float> m_cooldowns;  // �÷��̾ ��ٿ� ����
    size_t m_triggerEventId;
};