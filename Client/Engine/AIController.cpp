#include <limits>

#include "pch.h"
#include "AIController.h"
#include "GameObject.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Transform.h"
#include "Timer.h"
#include "Animator.h"



void AIController::Update()
{
	if (_target.expired()) // 타겟이 없는 경우
	{
		SetChangeState(MonsterState::IDLE);
		FindTarget(L"Player");

	}
	else
	{
		MoveToTarget();
	}

	// 현재 상태와 이전상태가 바뀌었다면 애니메이션을 플레이
	if (_currentState != _previousState)
	{
		PlayAnimation(_monsterType);
	}

	
}

void AIController::SetChangeState(MonsterState state)
{
	_previousState = _currentState;
	_currentState = state;

}

void AIController::MoveToTarget() // AI가 타겟을 향해 이동하는 함수
{

	if (!_target.expired()) // 타겟이 존재하는 경우
	{
		if (isAttackCount <= 0)
		{
			_moveSpeed = 100.f;
			SetChangeState(MonsterState::MOVE);
		}
		else 
		{
			SetChangeState(MonsterState::ATTACK);
		}

		Vec3 targetPosition = _target.lock()->GetTransform()->GetLocalPosition();
		Vec3 direction = CalculateDirectionToTarget(targetPosition);

		// 프레임마다 타겟을 향해 이동
		GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition() + direction * _moveSpeed * DELTA_TIME);

		// 타겟을 향해 회전
		GetTransform()->LookAt(-direction);

		float distanceToTarget = CalculateDistanceToTarget(targetPosition);
		
		 // 타겟이 공격범위에 들어오고 쿨타임이 지났다면 공격
		if (distanceToTarget < _attackRange && isAttackCount <=  0)
		{
			isAttackCount =50;
			_moveSpeed = 0.f;
			_target.reset();
			AttackPlayer();
			_currentState = MonsterState::ATTACK;
			PlayAnimation(_monsterType);
			return;
		}
		else if(distanceToTarget > _detectionRange * 2) // 타겟이 시야에서 벗어난 경우
		{
			_target.reset();
			_currentState = MonsterState::IDLE;
			PlayAnimation(_monsterType);
		}
		//printf("isAttackCount : %d\n", isAttackCount);
		isAttackCount--;

	}
}



void AIController::AttackPlayer()
{
	hp -= 1;


	printf("hp : %d\n", hp);
	
	// 차후 플레이어 피격처리 추가 

}

float AIController::CalculateDistanceToTarget(Vec3 TargetPos)
{
	Vec3 myPosition = GetTransform()->GetLocalPosition();
	return (myPosition - TargetPos).Length();

}

Vec3 AIController::CalculateDirectionToTarget(Vec3 TargetPos) // 타겟을 향한 방향을 계산하는 함수
{
	Vec3 myPosition = GetTransform()->GetLocalPosition();
	Vec3 direction = TargetPos - myPosition;
	direction.Normalize();
	return direction;

}

void AIController::FindTarget(wstring TargetName)
{
	auto scene = GET_SINGLE(SceneManager)->GetActiveScene();
	if (!scene)
		return;

	Vec3 myPosition = GetTransform()->GetLocalPosition();

	Vec3 TargetPoisition;
	for (auto& gameObject : scene->GetGameObjects())
	{
		if (gameObject->GetName().find(TargetName) != std::string::npos)
		{
			TargetPoisition = gameObject->GetTransform()->GetLocalPosition();
			if (CalculateDistanceToTarget(TargetPoisition) < _detectionRange)
			{
				SetTarget(gameObject);
			}
			break;
		}
	}
}
void AIController::PlayAnimation(MonsterType monsterType)
{
		// 애니메이션 플레이

	if (monsterType == MonsterType::TelepachyRat)
	{

		switch (_currentState)
		{
		case MonsterState::IDLE:
			GetAnimator()->Play(0);
			break;
		case MonsterState::MOVE:
			GetAnimator()->Play(19);
			break;
		case MonsterState::ATTACK:
			GetAnimator()->Play(2);
			break;
		case MonsterState::DEAD:
			GetAnimator()->Play(4);
			break;
		default:
			break;
		}
	}
	else if (monsterType == MonsterType::Haunt)
	{
		switch (_currentState)
		{
		case MonsterState::IDLE:
			GetAnimator()->Play(1);
			break;
		case MonsterState::MOVE:
			GetAnimator()->Play(4);
			break;
		case MonsterState::ATTACK:
			GetAnimator()->Play(6);
			break;
		case MonsterState::DEAD:
			GetAnimator()->Play(16);
			break;
		default:
			break;
		}
	}
}

