#pragma once
#include "Event.h"
#include "PhysicsTypes.h"
#include "GameObject.h"
#include "Transform.h"
#include "AttackInfo.h"

enum class PLAYER_STATE;
class Player;
namespace Event
{
    // ���� �浹 �̺�Ʈ
    struct CollisionEvent : public IEvent {
		CollisionEvent() = default;
        CollisionEvent(const PxActor* actor1, const PxActor* actor2,
            const PxVec3& position, const PxVec3& normal, float impulse)
            : actor1(actor1)
            , actor2(actor2)
            , position(position)
            , normal(normal)
            , impulse(impulse) {}

        const PxActor* actor1;
        const PxActor* actor2;
        PxVec3 position;
        PxVec3 normal;
        float impulse;
    };

    // �Է� �̺�Ʈ
    struct InputEvent : public IEvent {
        enum class Type {
            KeyDown,
			KeyRepeat,
            KeyUp,
            MouseMove,
            MouseButtonDown,
            MouseButtonUp
        };
		InputEvent() = default;
        InputEvent(Type type, int code, float x = 0.0f, float y = 0.0f)
            : type(type)
            , code(code)
            , x(x)
            , y(y) {}

        Type type;
        int code;
        float x, y;
    };

	// �÷��̾� ���� ���� �̺�Ʈ
    struct PlayerStateChangeEvent : public Event::IEvent {
        PlayerStateChangeEvent(PLAYER_STATE prevState, PLAYER_STATE newState, GameObject* player)
            : prevState(prevState), newState(newState), player(player) {
        }

        PLAYER_STATE prevState;
        PLAYER_STATE newState;
        GameObject* player;
    };

    // ����ü �浹 �̺�Ʈ
    struct ProjectileHitEvent : public Event::IEvent {
		ProjectileHitEvent() = default;
        ProjectileHitEvent(const Vec3& hitPosition, const Vec3& hitNormal,
            GameObject* projectile, GameObject* target)
            : hitPosition(hitPosition), hitNormal(hitNormal)
            , projectile(projectile), target(target) {
        }

        Vec3 hitPosition;
        Vec3 hitNormal;
        GameObject* projectile;
        GameObject* target;
    };

    // �ڷ���Ʈ �̺�Ʈ
    struct TeleportEvent : public Event::IEvent {
        enum class Phase { BEGIN, COMPLETE };

		TeleportEvent() = default;
        TeleportEvent(GameObject* player, const Vec3& fromPos, const Vec3& toPos, Phase phase)
            : player(player), fromPosition(fromPos), toPosition(toPos), phase(phase) {
        }

        GameObject* player;
        Vec3 fromPosition;
        Vec3 toPosition;
        Phase phase;
    };

    // �÷��̾� ��Ʈ �̺�Ʈ
    struct PlayerHitEvent : public Event::IEvent {
        PlayerHitEvent() = default;
        PlayerHitEvent(Player* player, GameObject* attacker, 
            float damage, const Vec3& hitCenter)
			: player(player), attackInfo(attackInfo), damage(damage), hitCenter(hitCenter) {
        }
		Player* player;
        AttackInfo* attackInfo;
		float damage;
		Vec3 hitCenter;
    };

    // Ʈ���� �̺�Ʈ
    struct TriggerEvent : public IEvent {
		TriggerEvent() = default;
        TriggerEvent(PxActor* triggerActor, PxActor* otherActor,
            bool triggerEnter)
            : triggerActor(triggerActor)
            , otherActor(otherActor)
            , triggerEnter(triggerEnter) {
        }

        PxActor* triggerActor;    // Ʈ���� Shape
        PxActor* otherActor;      // �ٸ� Actor
        bool triggerEnter;        // true�� ����, false�� ��Ż
    };
}