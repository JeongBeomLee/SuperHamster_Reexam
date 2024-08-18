#pragma once
#include "MonoBehaviour.h"
#include "TestAnimation.h"

enum class MonsterState
{
	IDLE,
	MOVE,
	ATTACK,
	DEAD,
    HIT,
};

enum class MonsterType
{
	TelepachyRat, // 텔레파시 쥐 
	Haunt, // 귀신

};



class GameObject;

class AIController : public MonoBehaviour
{
public:
   
    virtual void Update() override;
    void SetTarget(const std::shared_ptr<GameObject>& target) { _target = target; }
    void SetAIId(uint32_t id) { _aiId = id; }
    void SetMonsterType(MonsterType monsterType) { _monsterType = monsterType; }
    void SetChangeState(MonsterState state);

private:

    void MoveToTarget();
    void AttackPlayer();
   
    // Target과의 거리를 계산하는 함수
    float CalculateDistanceToTarget(Vec3 TargetPos);
    Vec3 CalculateDirectionToTarget(Vec3 TargetPos);

    // Scene에서 Target을 찾는 함수
    void FindTarget(wstring TargetName);

    void PlayAnimation(MonsterType monsterType);

private:

    std::weak_ptr<GameObject> _target;
    uint32_t _aiId;

    MonsterState _currentState = MonsterState::IDLE;
    MonsterState _previousState = _currentState;

    MonsterType _monsterType = MonsterType::TelepachyRat;
    
    float _moveSpeed = 100.f;
    float _attackRange = 120.f;
    float _detectionRange = 250.f;
    float _attackCooldown = 5.f;
    float _lastAttackTime = 0.0f;
    
    int isAttackCount = 0;
    int hp = 5;
};