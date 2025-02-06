#include "pch.h"
#include "ContactReportCallback.h"
#include "Logger.h"
//#include "EventManager.h"
#include "PhysicsTypes.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "PhysicsBody.h"

void ContactReportCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
    for (PxU32 i = 0; i < nbPairs; i++) {
        const PxContactPair& cp = pairs[i];

        // 충돌 이벤트 타입 확인
        if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
            PxActor* actor0 = pairHeader.actors[0];
            PxActor* actor1 = pairHeader.actors[1];

            // 충돌 시작
            Logger::Instance().Info("'{}' 와 '{}'가 충돌 발생.",
                actor0->getName(),
                actor1->getName());

            // 접촉점 정보 추출
            PxContactPairPoint contacts[1];
            PxU32 nbContacts = cp.extractContacts(contacts, 1);

            // 충돌 이벤트 발행
            /*if (nbContacts > 0) {
                Event::CollisionEvent event(
                    pairHeader.actors[0],
                    pairHeader.actors[1],
                    contacts[0].position,
                    contacts[0].normal,
                    contacts[0].impulse.magnitude()
                );

                EventManager::Instance().Publish(event);
            }*/

            // 충돌한 두 액터 중 하나가 Projectile 그룹인지 확인
			auto object0 = static_cast<GameObject*>(actor0->userData);
			auto object1 = static_cast<GameObject*>(actor1->userData);
            bool isProjectile0 =
                static_cast<PxU32>(object0->GetPhysicsBody()->GetCollisionGroup()) & static_cast<PxU32>(CollisionGroup::Projectile);
            bool isProjectile1 =
                static_cast<PxU32>(object1->GetPhysicsBody()->GetCollisionGroup()) & static_cast<PxU32>(CollisionGroup::Projectile);
            

            // 총알이 관련된 충돌인 경우
            if (isProjectile0 || isProjectile1) {
                // 충돌 지점 정보 추출
                PxContactPairPoint contactPoints[1];
                cp.extractContacts(contactPoints, 1);

                // 총알 GameObject 찾기
				auto projectileObject = isProjectile0 ? object0 : object1;
                projectileObject->SetActive(false);

                Logger::Instance().Info("충돌한 투사체 삭제. ID: {}", projectileObject->GetID());

                // TODO: 충돌 이펙트 생성
                // 여기에 파티클 시스템을 사용한 충돌 이펙트 생성 코드 추가
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