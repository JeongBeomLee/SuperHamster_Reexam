#include "pch.h"
#include "CharacterControllerHitReport.h"
#include "PhysicsTypes.h"
//#include "EventManager.h"
#include "Logger.h"

void CharacterControllerHitReport::onShapeHit(const PxControllerShapeHit& hit)
{
    // �Ϲ� ���� ��ü���� �浹
    /*Event::CollisionEvent event(
        hit.controller->getActor(),
        hit.shape->getActor(),
        PxVec3(static_cast<float>(hit.worldPos.x),
            static_cast<float>(hit.worldPos.y),
            static_cast<float>(hit.worldPos.z)),
        hit.worldNormal,
        hit.length
    );

    EventManager::Instance().Publish(event);*/

    /*Logger::Instance().Debug("ĳ���Ͱ� ���� ��ü�� �浹. �浹 ��ġ: ({}, {}, {})",
        hit.worldPos.x, hit.worldPos.y, hit.worldPos.z);*/
}

void CharacterControllerHitReport::onControllerHit(const PxControllersHit& hit)
{
    // �ٸ� ĳ���� ��Ʈ�ѷ����� �浹
    //Event::CollisionEvent event(
    //    hit.controller->getActor(),
    //    hit.other->getActor(),
    //    PxVec3(static_cast<float>(hit.worldPos.x),
    //        static_cast<float>(hit.worldPos.y),
    //        static_cast<float>(hit.worldPos.z)),
    //    PxVec3(0.0f),  // ��Ʈ�ѷ� �浹������ normal�� �������� ����
    //    0.0f           // ��ݷ��� �������� ����
    //);

    //EventManager::Instance().Publish(event);

    Logger::Instance().Debug("ĳ���Ͱ� �ٸ� ĳ���Ϳ� �浹. �浹 ��ġ: ({}, {}, {})",
        hit.worldPos.x, hit.worldPos.y, hit.worldPos.z);
}

void CharacterControllerHitReport::onObstacleHit(const PxControllerObstacleHit& hit)
{
    // ��ֹ����� �浹
    //Event::CollisionEvent event(
    //    hit.controller->getActor(),
    //    nullptr,  // ��ֹ��� PxActor�� ����
    //    PxVec3(static_cast<float>(hit.worldPos.x),
    //        static_cast<float>(hit.worldPos.y),
    //        static_cast<float>(hit.worldPos.z)),
    //    hit.worldNormal,
    //    0.0f      // ��ݷ��� �������� ����
    //);

    //EventManager::Instance().Publish(event);

    Logger::Instance().Debug("ĳ���Ͱ� ��ֹ��� �浹. �浹 ��ġ: ({}, {}, {})",
        hit.worldPos.x, hit.worldPos.y, hit.worldPos.z);
}
