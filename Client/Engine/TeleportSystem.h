#pragma once
#include "PhysicsTypes.h"
#include "GameObject.h"
#include "PhysicsObject.h"

// 텔레포트 정보를 담는 구조체
struct TeleportZone {
    TeleportZoneId id;           // 텔레포트 존 고유 ID
    Vec3 sourcePosition;         // 출발 지점 위치
    Vec3 destinationPosition;    // 도착 지점 위치
    Vec3 triggerSize;            // 트리거 크기
    string sourceName;          // 출발 지점 이름
    string destinationName;     // 도착 지점 이름

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

    // 텔레포트 존 생성 및 관리
    void CreateTeleportZones();
    const TeleportZone* GetTeleportZone(TeleportZoneId id) const;

    // 텔레포트 실행
    void TriggerTeleport(GameObject* player, TeleportZoneId id);

    // 디버깅용
    void DebugTeleportZones();

private:
    // 텔레포트 트리거 생성 유틸리티
    std::shared_ptr<GameObject> CreateTeleportTrigger(
        const Vec3& position,
        const Vec3& size,
        TeleportZoneId id);
    bool IsInCooldown(GameObject* player) const;
    void StartCooldown(GameObject* player);

private:
    const float TELEPORT_COOLDOWN = 1.0f;  // 1초의 쿨다운 시간
    std::unordered_map<TeleportZoneId, TeleportZone> m_teleportZones;
    std::unordered_map<GameObject*, float> m_cooldowns;  // 플레이어별 쿨다운 관리
    size_t m_triggerEventId;
};