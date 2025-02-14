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
        // ����ü�� �������� �浹 �˻�
        auto projectile = shapeObject->GetMonoBehaviour<Projectile>();
        auto cactus = controllerObject->GetMonoBehaviour<Cactus>();

        if (!projectile && !cactus) {
            // ������ �ٲ㼭 �ٽ� Ȯ��
            projectile = controllerObject->GetMonoBehaviour<Projectile>();
            cactus = shapeObject->GetMonoBehaviour<Cactus>();
        }

        if (projectile && cactus) {
            // �浹 ��ġ ���� ��ȯ
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

            // �浹 �̺�Ʈ �߻�
            Event::ProjectileHitEvent event(
                hitPosition,
                hitNormal,
                projectile->GetGameObject().get(),
                cactus->GetGameObject().get()
            );

            GET_SINGLE(EventManager)->Publish(event);
            Logger::Instance().Debug("������� ����ü �浹 �߻�. ��ġ: ({}, {}, {})",
                hitPosition.x, hitPosition.y, hitPosition.z);

            // ����ü ����
            projectile->GetGameObject()->SetActive(false);
        }
    }
#pragma endregion

#pragma region chestMonster and projectile
    {
        // ����ü�� �̹��� �浹 �˻�
        auto projectile = shapeObject->GetMonoBehaviour<Projectile>();
        auto chest = controllerObject->GetMonoBehaviour<ChestMonster>();

        if (!projectile && !chest) {
            // ������ �ٲ㼭 �ٽ� Ȯ��
            projectile = controllerObject->GetMonoBehaviour<Projectile>();
            chest = shapeObject->GetMonoBehaviour<ChestMonster>();
        }

        if (projectile && chest) {
            // �浹 ��ġ ���� ��ȯ
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

            // �浹 �̺�Ʈ �߻�
            Event::ProjectileHitEvent event(
                hitPosition,
                hitNormal,
                projectile->GetGameObject().get(),
                chest->GetGameObject().get()
            );

            GET_SINGLE(EventManager)->Publish(event);
            Logger::Instance().Debug("�̹Ͱ� ����ü �浹 �߻�. ��ġ: ({}, {}, {})",
                hitPosition.x, hitPosition.y, hitPosition.z);

            // ����ü ����
            projectile->GetGameObject()->SetActive(false);
        }
    }
#pragma endregion

#pragma region chestMonster and projectile
    {
        // ����ü�� �̹��� �浹 �˻�
        auto projectile = shapeObject->GetMonoBehaviour<Projectile>();
        auto bunnyRat = controllerObject->GetMonoBehaviour<BunnyRat>();

        if (!projectile && !bunnyRat) {
            // ������ �ٲ㼭 �ٽ� Ȯ��
            projectile = controllerObject->GetMonoBehaviour<Projectile>();
            bunnyRat = shapeObject->GetMonoBehaviour<BunnyRat>();
        }

        if (projectile && bunnyRat) {
            // �浹 ��ġ ���� ��ȯ
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

            // �浹 �̺�Ʈ �߻�
            Event::ProjectileHitEvent event(
                hitPosition,
                hitNormal,
                projectile->GetGameObject().get(),
                bunnyRat->GetGameObject().get()
            );

            GET_SINGLE(EventManager)->Publish(event);
            Logger::Instance().Debug("�̹Ͱ� ����ü �浹 �߻�. ��ġ: ({}, {}, {})",
                hitPosition.x, hitPosition.y, hitPosition.z);

            // ����ü ����
            projectile->GetGameObject()->SetActive(false);
        }
    }
#pragma endregion

#pragma region Boss and projectile
    {
        // ����ü�� �̹��� �浹 �˻�
        auto projectile = shapeObject->GetMonoBehaviour<Projectile>();
        auto boss = controllerObject->GetMonoBehaviour<Boss>();

        if (!projectile && !boss) {
            // ������ �ٲ㼭 �ٽ� Ȯ��
            projectile = controllerObject->GetMonoBehaviour<Projectile>();
            boss = shapeObject->GetMonoBehaviour<Boss>();
        }

        if (projectile && boss) {
            // �浹 ��ġ ���� ��ȯ
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

            // �浹 �̺�Ʈ �߻�
            Event::ProjectileHitEvent event(
                hitPosition,
                hitNormal,
                projectile->GetGameObject().get(),
                boss->GetGameObject().get()
            );

            GET_SINGLE(EventManager)->Publish(event);
            Logger::Instance().Debug("������ ����ü �浹 �߻�. ��ġ: ({}, {}, {})",
                hitPosition.x, hitPosition.y, hitPosition.z);

            // ����ü ����
            projectile->GetGameObject()->SetActive(false);
        }
    }
#pragma endregion
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
