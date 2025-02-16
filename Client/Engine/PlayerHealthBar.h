#pragma once
#include "MonoBehaviour.h"
class PlayerHealthBar : public MonoBehaviour
{
public:
    PlayerHealthBar();
    virtual ~PlayerHealthBar();

    virtual void Start() override;
    virtual void Update() override;

    void SetMaxHealth(float maxHealth) { _maxHealth = maxHealth; }
    void SetCurrentHealth(float currentHealth);

private:
    shared_ptr<GameObject> _outLineBar;         // 체력바 채우기 오브젝트
    shared_ptr<GameObject> _fillBar;         // 체력바 채우기 오브젝트

	Vec3 _fillBarScale = Vec3(285.f, 25.f, 1.f);
	Vec3 _fillBarPosition = Vec3(-465.f, 315.f, 500.f);

    float _maxHealth = 500.0f;
    float _currentHealth = 500.0f;
};

