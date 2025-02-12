#include "pch.h"
#include "ChestMonsterAwakeState.h"
#include "ChestMonster.h"
#include "Animator.h"

void ChestMonsterAwakeState::Enter(ChestMonster* chestMonster)
{
    chestMonster->PlayAnimation(CHEST_MONSTER_STATE::AWAKE);
    m_hasCheckedAnimation = false;
}

void ChestMonsterAwakeState::Update(ChestMonster* chestMonster, float deltaTime)
{
    auto animator = chestMonster->GetAnimator();
    if (!animator) return;

    // 애니메이션이 끝났는지 체크
    if (!m_hasCheckedAnimation && animator->IsAnimationFinished()) {
        m_hasCheckedAnimation = true;
        chestMonster->SetState(CHEST_MONSTER_STATE::IDLE);
        Logger::Instance().Debug("미믹이 깨어났습니다.");
    }
}

void ChestMonsterAwakeState::Exit(ChestMonster* chestMonster)
{
}
