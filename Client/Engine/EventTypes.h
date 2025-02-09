#pragma once
#include "Event.h"
#include "PhysicsTypes.h"

namespace Event
{
    // 물리 충돌 이벤트
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

    // 입력 이벤트
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
}