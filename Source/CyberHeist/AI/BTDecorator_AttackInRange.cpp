// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator_AttackInRange.h"
#include "CHAI.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/CHCharacterAIInterface.h"


UBTDecorator_AttackInRange::UBTDecorator_AttackInRange()
{
	NodeName = TEXT("CanAttack");
}

bool UBTDecorator_AttackInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return false;
	}

	ICHCharacterAIInterface* AIPawn = Cast<ICHCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return false;
	}

	// 블랙보드의 Target값을 가져온다. 
	APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGETACTOR));
	if (nullptr == Target)
	{
		return false;
	}
	
	// 구한 Target으로 거리를 구한다. 
	float DistanceToTarget = ControllingPawn->GetDistanceTo(Target);
	// 공격 범위
	float AttackRangeWithRadius = AIPawn->GetAIAttackRange();
	bResult = (DistanceToTarget <= AttackRangeWithRadius);	// (Target과의 거리 < 공격 범위) 이면 공격 

	UE_LOG(LogTemp, Log, TEXT("DistanceToTarget : %f	AttackRangeWithRadius : %f	bResult : %d"),DistanceToTarget, AttackRangeWithRadius, bResult );
	return bResult;	
}
