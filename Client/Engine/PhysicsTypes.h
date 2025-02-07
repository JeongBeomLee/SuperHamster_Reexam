#pragma once
#include "pch.h"

enum class PhysicsShapeType {
    Box,
    Sphere,
    Capsule,
    TriangleMesh
};

enum class CollisionGroup : PxU32 {
    Default = (1 << 0),     // 기본 그룹
    Character = (1 << 1),   // 플레이어
    Enemy = (1 << 2),       // 적
    Projectile = (1 << 3),  // 발사체
    Ground = (1 << 4),      // 지형
    Trigger = (1 << 5),     // 트리거 영역
    Obstacle = (1 << 6)
};

// 비트 연산자 오버로딩
inline CollisionGroup operator|(CollisionGroup a, CollisionGroup b) {
    return static_cast<CollisionGroup>(
        static_cast<PxU32>(a) | static_cast<PxU32>(b));
}

inline CollisionGroup operator&(CollisionGroup a, CollisionGroup b) {
    return static_cast<CollisionGroup>(
        static_cast<PxU32>(a) & static_cast<PxU32>(b));
}

inline CollisionGroup& operator|=(CollisionGroup& a, CollisionGroup b) {
    a = a | b;
    return a;
}

inline CollisionGroup& operator&=(CollisionGroup& a, CollisionGroup b) {
    a = a & b;
    return a;
}

enum class TeleportZoneId {
    NONE = 0,
    // Stage 1 텔레포트
    STAGE1_TO_STAGE2,  // Stage1 -> Stage2

    // Stage 2 텔레포트
    STAGE2_TO_STAGE1,  // Stage2 -> Stage1
    STAGE2_TO_STAGE3,  // Stage2 -> Stage3
    STAGE2_TO_STAGE9,  // Stage2 -> Stage9
    STAGE2_TO_STAGE6,  // Stage2 -> Stage6

    // Stage 3 텔레포트
    STAGE3_TO_STAGE2,  // Stage3 -> Stage2
    STAGE3_TO_STAGE4,  // Stage3 -> Stage4
    STAGE3_TO_STAGE5,  // Stage3 -> Stage5

    // Stage 4 텔레포트
    STAGE4_TO_STAGE3,  // Stage4 -> Stage3

    // Stage 5 텔레포트
    STAGE5_TO_STAGE3,  // Stage5 -> Stage3

    // Stage 6 텔레포트
    STAGE6_TO_STAGE2,  // Stage6 -> Stage2
    STAGE6_TO_STAGE7,  // Stage6 -> Stage7

    // Stage 7 텔레포트
    STAGE7_TO_STAGE6,  // Stage7 -> Stage6
    STAGE7_TO_STAGE8,  // Stage7 -> Stage8

    // Stage 8 텔레포트
    STAGE8_TO_STAGE7,  // Stage8 -> Stage7

    // Stage 9 텔레포트
    STAGE9_TO_STAGE2,  // Stage9 -> Stage2
};