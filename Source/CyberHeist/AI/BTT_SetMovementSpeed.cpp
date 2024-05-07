// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_SetMovementSpeed.h"
#include "AIController.h"
#include "CHAI.h"
#include "CHAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/CHCharacterNonPlayer.h"
#include "Interface/CHCharacterAIInterface.h"

UBTT_SetMovementSpeed::UBTT_SetMovementSpeed()
{
	// MovementSpeed = ECHAIMovementSpeed::Idle;
}

EBTNodeResult::Type UBTT_SetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	
	AIPawn->SetMovementSpeed(MovementSpeed);

	return EBTNodeResult::Succeeded;
}
