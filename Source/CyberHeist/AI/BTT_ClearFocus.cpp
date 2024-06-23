// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_ClearFocus.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/CHCharacterAIInterface.h"

UBTT_ClearFocus::UBTT_ClearFocus()
{
	
}

EBTNodeResult::Type UBTT_ClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(FocusTarget.SelectedKeyName, nullptr);	
	// OwnerController->ClearFocus(EAIFocusPriority::Gameplay);

	return EBTNodeResult::Succeeded;
}
