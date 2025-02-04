#include "pch.h"
#include "ContactReportCallback.h"
#include "Logger.h"
//#include "EventManager.h"

void ContactReportCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
    for (PxU32 i = 0; i < nbPairs; i++) {
        const PxContactPair& cp = pairs[i];

        // �浹 �̺�Ʈ Ÿ�� Ȯ��
        if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
            // �浹 ����
            Logger::Instance().Info("'{}' �� '{}'�� �浹 �߻�.",
                pairHeader.actors[0]->getName(),
                pairHeader.actors[1]->getName());

            // ������ ���� ����
            PxContactPairPoint contacts[1];
            PxU32 nbContacts = cp.extractContacts(contacts, 1);

            // �浹 �̺�Ʈ ����
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
        }
        else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST) {
            // �浹 ����
            Logger::Instance().Info("'{}' �� '{}'�� �浹 ����.",
                pairHeader.actors[0]->getName(),
                pairHeader.actors[1]->getName());
        }
    }
}