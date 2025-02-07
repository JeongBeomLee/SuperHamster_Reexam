#pragma once
#include "pch.h"

enum class PhysicsShapeType {
    Box,
    Sphere,
    Capsule,
    TriangleMesh
};

enum class CollisionGroup : PxU32 {
    Default = (1 << 0),     // �⺻ �׷�
    Character = (1 << 1),   // �÷��̾�
    Enemy = (1 << 2),       // ��
    Projectile = (1 << 3),  // �߻�ü
    Ground = (1 << 4),      // ����
    Trigger = (1 << 5),     // Ʈ���� ����
    Obstacle = (1 << 6)
};

// ��Ʈ ������ �����ε�
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
    // Stage 1 �ڷ���Ʈ
    STAGE1_TO_STAGE2,  // Stage1 -> Stage2

    // Stage 2 �ڷ���Ʈ
    STAGE2_TO_STAGE1,  // Stage2 -> Stage1
    STAGE2_TO_STAGE3,  // Stage2 -> Stage3
    STAGE2_TO_STAGE9,  // Stage2 -> Stage9
    STAGE2_TO_STAGE6,  // Stage2 -> Stage6

    // Stage 3 �ڷ���Ʈ
    STAGE3_TO_STAGE2,  // Stage3 -> Stage2
    STAGE3_TO_STAGE4,  // Stage3 -> Stage4
    STAGE3_TO_STAGE5,  // Stage3 -> Stage5

    // Stage 4 �ڷ���Ʈ
    STAGE4_TO_STAGE3,  // Stage4 -> Stage3

    // Stage 5 �ڷ���Ʈ
    STAGE5_TO_STAGE3,  // Stage5 -> Stage3

    // Stage 6 �ڷ���Ʈ
    STAGE6_TO_STAGE2,  // Stage6 -> Stage2
    STAGE6_TO_STAGE7,  // Stage6 -> Stage7

    // Stage 7 �ڷ���Ʈ
    STAGE7_TO_STAGE6,  // Stage7 -> Stage6
    STAGE7_TO_STAGE8,  // Stage7 -> Stage8

    // Stage 8 �ڷ���Ʈ
    STAGE8_TO_STAGE7,  // Stage8 -> Stage7

    // Stage 9 �ڷ���Ʈ
    STAGE9_TO_STAGE2,  // Stage9 -> Stage2
};