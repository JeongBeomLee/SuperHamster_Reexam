#pragma once
#include "pch.h"
#include "GameObject.h"
struct AttackInfo {
    Vec3 center;          // 공격 중심점
    float radius;         // 공격 범위
    float damage;         // 공격력
    GameObject* attacker; // 공격자

    AttackInfo(const Vec3& center, float radius, float damage, GameObject* attacker)
        : center(center), radius(radius), damage(damage), attacker(attacker) {
    }
};