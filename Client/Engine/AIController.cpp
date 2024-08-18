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
	if (_target.expired()) // Ÿ���� ���� ���
	{
		SetChangeState(MonsterState::IDLE);
		FindTarget(L"Player");

	}
	else
	{
		MoveToTarget();
	}

	// ���� ���¿� �������°� �ٲ���ٸ� �ִϸ��̼��� �÷���
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

void AIController::MoveToTarget() // AI�� Ÿ���� ���� �̵��ϴ� �Լ�
{

	if (!_target.expired()) // Ÿ���� �����ϴ� ���
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

		// �����Ӹ��� Ÿ���� ���� �̵�
		GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition() + direction * _moveSpeed * DELTA_TIME);

		// Ÿ���� ���� ȸ��
		GetTransform()->LookAt(-direction);

		float distanceToTarget = CalculateDistanceToTarget(targetPosition);
		
		 // Ÿ���� ���ݹ����� ������ ��Ÿ���� �����ٸ� ����
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
		else if(distanceToTarget > _detectionRange * 2) // Ÿ���� �þ߿��� ��� ���
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
	
	// ���� �÷��̾� �ǰ�ó�� �߰� 

}

float AIController::CalculateDistanceToTarget(Vec3 TargetPos)
{
	Vec3 myPosition = GetTransform()->GetLocalPosition();
	return (myPosition - TargetPos).Length();

}

Vec3 AIController::CalculateDirectionToTarget(Vec3 TargetPos) // Ÿ���� ���� ������ ����ϴ� �Լ�
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
		// �ִϸ��̼� �÷���

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

