#pragma once
#include "MonoBehaviour.h"
class BossHealthBar : public MonoBehaviour
{
public:
    BossHealthBar();
    virtual ~BossHealthBar();

    virtual void Start() override;
    virtual void Update() override;

    void SetMaxHealth(float maxHealth) { _maxHealth = maxHealth; }
    void SetCurrentHealth(float currentHealth);
    void Hide() { _fillBarPosition.z = -100.f; }
	void Show() { _fillBarPosition.z = 500.f; }

private:
    shared_ptr<GameObject> _fillBar;         // 체력바 채우기 오브젝트

    Vec3 _fillBarScale = Vec3(800.f, 30.f, 1.f);
    Vec3 _fillBarPosition = Vec3(0.f, -300.f, -100.f);

    float _maxHealth = 1000.0f;
    float _currentHealth = 1000.0f;
};

