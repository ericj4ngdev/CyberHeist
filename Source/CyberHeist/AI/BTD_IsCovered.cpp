// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTD_IsCovered.h"
#include "AIController.h"
#include "Character/CHCharacterBase.h"

UBTD_IsCovered::UBTD_IsCovered()
{
}

bool UBTD_IsCovered::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	// return Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return false;
	}

	const bool bResult = Cast<ACHCharacterBase>(ControllingPawn)->GetCovered() == 1;
	return bResult;
}
