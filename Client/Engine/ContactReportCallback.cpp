#include "pch.h"
#include "ContactReportCallback.h"
#include "Logger.h"
#include "PhysicsTypes.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "PhysicsBody.h"
#include "ProjectileManager.h"
#include "TeleportSystem.h"
#include "Resources.h"
#include "SoundSystem.h"

void ContactReportCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
    for (PxU32 i = 0; i < nbPairs; i++) {
        const PxContactPair& cp = pairs[i];

        // �浹 �̺�Ʈ Ÿ�� Ȯ��
        if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
            PxActor* actor0 = pairHeader.actors[0];
            PxActor* actor1 = pairHeader.actors[1];

            Logger::Instance().Info("'{}' �� '{}'�� �浹 �߻�.",
                actor0->getName(),
                actor1->getName()
            );

            // ������ ���� ����
            PxContactPairPoint contacts[1];
            PxU32 nbContacts = cp.extractContacts(contacts, 1);

            if (nbContacts > 0) {
                // �浹 ����
                Logger::Instance().Info("�浹 ��ġ ({}, {}, {})",
                    contacts[0].position.x,
                    contacts[0].position.y,
                    contacts[0].position.z
                );

                // �浹�� �� ���� �� �ϳ��� Projectile �׷����� Ȯ��
                auto object0 = static_cast<GameObject*>(actor0->userData);
                auto object1 = static_cast<GameObject*>(actor1->userData);
                bool isProjectile0 =
                    static_cast<PxU32>(object0->GetPhysicsBody()->GetCollisionGroup()) & static_cast<PxU32>(CollisionGroup::Projectile);
                bool isProjectile1 =
                    static_cast<PxU32>(object1->GetPhysicsBody()->GetCollisionGroup()) & static_cast<PxU32>(CollisionGroup::Projectile);


                // �Ѿ��� ���õ� �浹�� ���
                if (isProjectile0 || isProjectile1) {
                    // �Ѿ� GameObject ã��
                    auto projectileObject = isProjectile0 ? object0 : object1;
                    projectileObject->SetActive(false);

                    Logger::Instance().Info("�浹�� ����ü ����. ID: {}", projectileObject->GetID());

                    Vec3 collisionPos(contacts[0].position.x, contacts[0].position.y, contacts[0].position.z);
                    GET_SINGLE(ProjectileManager)->PlayCollisionEffect(collisionPos);

                    // �Ѿ� �ǰ� ���� ���
                    auto sound = GET_SINGLE(Resources)->Get<Sound>(L"LaserHit");
                    if (sound) {
                        sound->SetVolume(50.f);
                        GET_SINGLE(SoundSystem)->Play3D(sound, collisionPos);
                    }
                }
                else {
                    Logger::Instance().Warning("'{}' �� '{}'�� �浹���� �������� ã�� �� �����ϴ�.",
                        actor0->getName(),
                        actor1->getName());
                }
            }
        }
        else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST) {
            // �浹 ����
            Logger::Instance().Info("'{}' �� '{}'�� �浹 ����.",
                pairHeader.actors[0]->getName(),
                pairHeader.actors[1]->getName());
        }
    }
}

void ContactReportCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
    for (PxU32 i = 0; i < count; i++) {
        const PxTriggerPair& pair = pairs[i];

        // Ʈ���� �̺�Ʈ�� ��ȿ���� Ȯ��
        if (pair.triggerActor == nullptr || pair.otherActor == nullptr)
            continue;

        // �浹 ���� �̺�Ʈ�� ó��
        if (pair.status & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
            // Ʈ������ �ڷ���Ʈ ID Ȯ��
            auto* teleportId = static_cast<TeleportZoneId*>(pair.triggerActor->userData);
            if (!teleportId) {
                Logger::Instance().Warning("Ʈ���ſ� �ڷ���Ʈ ID�� ����");
                continue;
            }

            // �浹�� ���Ͱ� �÷��̾����� Ȯ��
            auto* gameObject = static_cast<GameObject*>(pair.otherActor->userData);
            if (!gameObject) {
                Logger::Instance().Warning("�浹 ��ü ������ ã�� �� ����");
                continue;
            }

            // CharacterController ������Ʈ Ȯ��
            if (gameObject->GetCharacterController()) {
                GET_SINGLE(TeleportSystem)->TriggerTeleport(gameObject, *teleportId);
            }
        }
    }
}
