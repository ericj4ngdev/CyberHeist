// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_SetStateAsPassive.h"
#include "AIController.h"
#include "CHAIControllerBase.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Interface/CHCharacterAIInterface.h"

UBTT_SetStateAsPassive::UBTT_SetStateAsPassive()
{
	
}

EBTNodeResult::Type UBTT_SetStateAsPassive::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}	
	
	ICHCharacterAIInterface* AIPawn = Cast<ICHCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* OwnerController = OwnerComp.GetAIOwner();
	// Cast<ACHAIController>(OwnerController)->SetStateAsPassive();
	
	return EBTNodeResult::Succeeded;
}
