#pragma once
#include "pch.h"
#include "GameObject.h"
struct AttackInfo {
    Vec3 center;          // ���� �߽���
    float radius;         // ���� ����
    float damage;         // ���ݷ�
    GameObject* attacker; // ������

    AttackInfo(const Vec3& center, float radius, float damage, GameObject* attacker)
        : center(center), radius(radius), damage(damage), attacker(attacker) {
    }
};