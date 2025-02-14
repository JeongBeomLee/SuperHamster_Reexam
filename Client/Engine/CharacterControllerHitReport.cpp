#include "pch.h"
#include "CharacterControllerHitReport.h"
#include "PhysicsTypes.h"
#include "Logger.h"
#include "GameObject.h"
#include "Projectile.h"
#include "Cactus.h"
#include "ChestMonster.h"
#include "BunnyRat.h"
#include "Boss.h"
#include "EventManager.h"

void CharacterControllerHitReport::onShapeHit(const PxControllerShapeHit& hit)
{
    GameObject* controllerObject = static_cast<GameObject*>(hit.controller->getActor()->userData);
    GameObject* shapeObject = static_cast<GameObject*>(hit.actor->userData);

    if (!controllerObject || !shapeObject) return;

#pragma region cactus and projectile
    {
        // 투사체와 선인장의 충돌 검사
        auto projectile = shapeObject->GetMonoBehaviour<Projectile>();
        auto cactus = controllerObject->GetMonoBehaviour<Cactus>();

        if (!projectile && !cactus) {
            // 순서를 바꿔서 다시 확인
            projectile = controllerObject->GetMonoBehaviour<Projectile>();
            cactus = shapeObject->GetMonoBehaviour<Cactus>();
        }

        if (projectile && cactus) {
            // 충돌 위치 정보 변환
            Vec3 hitPosition(
                static_cast<float>(hit.worldPos.x),
                static_cast<float>(hit.worldPos.y),
                static_cast<float>(hit.worldPos.z)
            );

            Vec3 hitNormal(
                static_cast<float>(hit.worldNormal.x),
                static_cast<float>(hit.worldNormal.y),
                static_cast<float>(hit.worldNormal.z)
            );

            // 충돌 이벤트 발생
            Event::ProjectileHitEvent event(
                hitPosition,
                hitNormal,
                projectile->GetGameObject().get(),
                cactus->GetGameObject().get()
            );

            GET_SINGLE(EventManager)->Publish(event);
            Logger::Instance().Debug("선인장과 투사체 충돌 발생. 위치: ({}, {}, {})",
                hitPosition.x, hitPosition.y, hitPosition.z);

            // 투사체 제거
            projectile->GetGameObject()->SetActive(false);
        }
    }
#pragma endregion

#pragma region chestMonster and projectile
    {
        // 투사체와 미믹의 충돌 검사
        auto projectile = shapeObject->GetMonoBehaviour<Projectile>();
        auto chest = controllerObject->GetMonoBehaviour<ChestMonster>();

        if (!projectile && !chest) {
            // 순서를 바꿔서 다시 확인
            projectile = controllerObject->GetMonoBehaviour<Projectile>();
            chest = shapeObject->GetMonoBehaviour<ChestMonster>();
        }

        if (projectile && chest) {
            // 충돌 위치 정보 변환
            Vec3 hitPosition(
                static_cast<float>(hit.worldPos.x),
                static_cast<float>(hit.worldPos.y),
                static_cast<float>(hit.worldPos.z)
            );

            Vec3 hitNormal(
                static_cast<float>(hit.worldNormal.x),
                static_cast<float>(hit.worldNormal.y),
                static_cast<float>(hit.worldNormal.z)
            );

            // 충돌 이벤트 발생
            Event::ProjectileHitEvent event(
                hitPosition,
                hitNormal,
                projectile->GetGameObject().get(),
                chest->GetGameObject().get()
            );

            GET_SINGLE(EventManager)->Publish(event);
            Logger::Instance().Debug("미믹과 투사체 충돌 발생. 위치: ({}, {}, {})",
                hitPosition.x, hitPosition.y, hitPosition.z);

            // 투사체 제거
            projectile->GetGameObject()->SetActive(false);
        }
    }
#pragma endregion

#pragma region chestMonster and projectile
    {
        // 투사체와 미믹의 충돌 검사
        auto projectile = shapeObject->GetMonoBehaviour<Projectile>();
        auto bunnyRat = controllerObject->GetMonoBehaviour<BunnyRat>();

        if (!projectile && !bunnyRat) {
            // 순서를 바꿔서 다시 확인
            projectile = controllerObject->GetMonoBehaviour<Projectile>();
            bunnyRat = shapeObject->GetMonoBehaviour<BunnyRat>();
        }

        if (projectile && bunnyRat) {
            // 충돌 위치 정보 변환
            Vec3 hitPosition(
                static_cast<float>(hit.worldPos.x),
                static_cast<float>(hit.worldPos.y),
                static_cast<float>(hit.worldPos.z)
            );

            Vec3 hitNormal(
                static_cast<float>(hit.worldNormal.x),
                static_cast<float>(hit.worldNormal.y),
                static_cast<float>(hit.worldNormal.z)
            );

            // 충돌 이벤트 발생
            Event::ProjectileHitEvent event(
                hitPosition,
                hitNormal,
                projectile->GetGameObject().get(),
                bunnyRat->GetGameObject().get()
            );

            GET_SINGLE(EventManager)->Publish(event);
            Logger::Instance().Debug("미믹과 투사체 충돌 발생. 위치: ({}, {}, {})",
                hitPosition.x, hitPosition.y, hitPosition.z);

            // 투사체 제거
            projectile->GetGameObject()->SetActive(false);
        }
    }
#pragma endregion

#pragma region Boss and projectile
    {
        // 투사체와 미믹의 충돌 검사
        auto projectile = shapeObject->GetMonoBehaviour<Projectile>();
        auto boss = controllerObject->GetMonoBehaviour<Boss>();

        if (!projectile && !boss) {
            // 순서를 바꿔서 다시 확인
            projectile = controllerObject->GetMonoBehaviour<Projectile>();
            boss = shapeObject->GetMonoBehaviour<Boss>();
        }

        if (projectile && boss) {
            // 충돌 위치 정보 변환
            Vec3 hitPosition(
                static_cast<float>(hit.worldPos.x),
                static_cast<float>(hit.worldPos.y),
                static_cast<float>(hit.worldPos.z)
            );

            Vec3 hitNormal(
                static_cast<float>(hit.worldNormal.x),
                static_cast<float>(hit.worldNormal.y),
                static_cast<float>(hit.worldNormal.z)
            );

            // 충돌 이벤트 발생
            Event::ProjectileHitEvent event(
                hitPosition,
                hitNormal,
                projectile->GetGameObject().get(),
                boss->GetGameObject().get()
            );

            GET_SINGLE(EventManager)->Publish(event);
            Logger::Instance().Debug("보스와 투사체 충돌 발생. 위치: ({}, {}, {})",
                hitPosition.x, hitPosition.y, hitPosition.z);

            // 투사체 제거
            projectile->GetGameObject()->SetActive(false);
        }
    }
#pragma endregion
}

void CharacterControllerHitReport::onControllerHit(const PxControllersHit& hit)
{
    // 다른 캐릭터 컨트롤러와의 충돌
    //Event::CollisionEvent event(
    //    hit.controller->getActor(),
    //    hit.other->getActor(),
    //    PxVec3(static_cast<float>(hit.worldPos.x),
    //        static_cast<float>(hit.worldPos.y),
    //        static_cast<float>(hit.worldPos.z)),
    //    PxVec3(0.0f),  // 컨트롤러 충돌에서는 normal이 제공되지 않음
    //    0.0f           // 충격량도 제공되지 않음
    //);

    //EventManager::Instance().Publish(event);

    Logger::Instance().Debug("캐릭터가 다른 캐릭터와 충돌. 충돌 위치: ({}, {}, {})",
        hit.worldPos.x, hit.worldPos.y, hit.worldPos.z);
}

void CharacterControllerHitReport::onObstacleHit(const PxControllerObstacleHit& hit)
{
    // 장애물과의 충돌
    //Event::CollisionEvent event(
    //    hit.controller->getActor(),
    //    nullptr,  // 장애물은 PxActor가 없음
    //    PxVec3(static_cast<float>(hit.worldPos.x),
    //        static_cast<float>(hit.worldPos.y),
    //        static_cast<float>(hit.worldPos.z)),
    //    hit.worldNormal,
    //    0.0f      // 충격량은 제공되지 않음
    //);

    //EventManager::Instance().Publish(event);

    Logger::Instance().Debug("캐릭터가 장애물과 충돌. 충돌 위치: ({}, {}, {})",
        hit.worldPos.x, hit.worldPos.y, hit.worldPos.z);
}
