#pragma once
// 동적/정적 액터들 간의 충돌 콜백
class ContactReportCallback : public PxSimulationEventCallback {
public:
    void onContact(const PxContactPairHeader& pairHeader,
        const PxContactPair* pairs, PxU32 nbPairs) override;

    void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override {}
    void onWake(PxActor** actors, PxU32 count) override {}
    void onSleep(PxActor** actors, PxU32 count) override {}
    void onTrigger(PxTriggerPair* pairs, PxU32 count) override;
    void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override {}
};