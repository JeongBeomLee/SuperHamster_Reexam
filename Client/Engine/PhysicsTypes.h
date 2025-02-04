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