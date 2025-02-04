#pragma once
// 컨트롤러/액터, 컨트롤러/컨트롤러, 컨트롤러/장애물 간의 충돌 콜백
class CharacterControllerHitReport : public PxUserControllerHitReport {
public:
    void onShapeHit(const PxControllerShapeHit& hit) override;
    void onControllerHit(const PxControllersHit& hit) override;
    void onObstacleHit(const PxControllerObstacleHit& hit) override;
};
