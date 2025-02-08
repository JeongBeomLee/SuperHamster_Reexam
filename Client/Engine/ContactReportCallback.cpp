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

        // 충돌 이벤트 타입 확인
        if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
            PxActor* actor0 = pairHeader.actors[0];
            PxActor* actor1 = pairHeader.actors[1];

            Logger::Instance().Info("'{}' 와 '{}'가 충돌 발생.",
                actor0->getName(),
                actor1->getName()
            );

            // 접촉점 정보 추출
            PxContactPairPoint contacts[1];
            PxU32 nbContacts = cp.extractContacts(contacts, 1);

            if (nbContacts > 0) {
                // 충돌 시작
                Logger::Instance().Info("충돌 위치 ({}, {}, {})",
                    contacts[0].position.x,
                    contacts[0].position.y,
                    contacts[0].position.z
                );

                // 충돌한 두 액터 중 하나가 Projectile 그룹인지 확인
                auto object0 = static_cast<GameObject*>(actor0->userData);
                auto object1 = static_cast<GameObject*>(actor1->userData);
                bool isProjectile0 =
                    static_cast<PxU32>(object0->GetPhysicsBody()->GetCollisionGroup()) & static_cast<PxU32>(CollisionGroup::Projectile);
                bool isProjectile1 =
                    static_cast<PxU32>(object1->GetPhysicsBody()->GetCollisionGroup()) & static_cast<PxU32>(CollisionGroup::Projectile);


                // 총알이 관련된 충돌인 경우
                if (isProjectile0 || isProjectile1) {
                    // 총알 GameObject 찾기
                    auto projectileObject = isProjectile0 ? object0 : object1;
                    projectileObject->SetActive(false);

                    Logger::Instance().Info("충돌한 투사체 삭제. ID: {}", projectileObject->GetID());

                    Vec3 collisionPos(contacts[0].position.x, contacts[0].position.y, contacts[0].position.z);
                    GET_SINGLE(ProjectileManager)->PlayCollisionEffect(collisionPos);

                    // 총알 피격 사운드 재생
                    auto sound = GET_SINGLE(Resources)->Get<Sound>(L"LaserHit");
                    if (sound) {
                        sound->SetVolume(50.f);
                        GET_SINGLE(SoundSystem)->Play3D(sound, collisionPos);
                    }
                }
                else {
                    Logger::Instance().Warning("'{}' 와 '{}'의 충돌에서 접촉점을 찾을 수 없습니다.",
                        actor0->getName(),
                        actor1->getName());
                }
            }
        }
        else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST) {
            // 충돌 종료
            Logger::Instance().Info("'{}' 와 '{}'가 충돌 종료.",
                pairHeader.actors[0]->getName(),
                pairHeader.actors[1]->getName());
        }
    }
}

void ContactReportCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
    for (PxU32 i = 0; i < count; i++) {
        const PxTriggerPair& pair = pairs[i];

        // 트리거 이벤트가 유효한지 확인
        if (pair.triggerActor == nullptr || pair.otherActor == nullptr)
            continue;

        // 충돌 시작 이벤트만 처리
        if (pair.status & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
            // 트리거의 텔레포트 ID 확인
            auto* teleportId = static_cast<TeleportZoneId*>(pair.triggerActor->userData);
            if (!teleportId) {
                Logger::Instance().Warning("트리거에 텔레포트 ID가 없음");
                continue;
            }

            // 충돌한 액터가 플레이어인지 확인
            auto* gameObject = static_cast<GameObject*>(pair.otherActor->userData);
            if (!gameObject) {
                Logger::Instance().Warning("충돌 객체 정보를 찾을 수 없음");
                continue;
            }

            // CharacterController 컴포넌트 확인
            if (gameObject->GetCharacterController()) {
                GET_SINGLE(TeleportSystem)->TriggerTeleport(gameObject, *teleportId);
            }
        }
    }
}
