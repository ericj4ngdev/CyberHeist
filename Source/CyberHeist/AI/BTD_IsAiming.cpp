// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTD_IsAiming.h"
#include "CHAI.h"
#include "AIController.h"
#include "CyberHeist.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/CHCharacterBase.h"
#include "Interface/CHCharacterAIInterface.h"

UBTD_IsAiming::UBTD_IsAiming()
{
	NodeName = "BTD_IsAiming";
}

bool UBTD_IsAiming::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	// bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	// CH_LOG(LogCHNetwork, Log, TEXT("Begin"))
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return false;
	}
	// CH_LOG(LogCHNetwork, Log, TEXT("End"))
	const bool bResult = Cast<ACHCharacterBase>(ControllingPawn)->GetAiming() == 1;
	return bResult;
}
