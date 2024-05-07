// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_Focus.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Interface/CHCharacterAIInterface.h"
// #include ""

UBTT_Focus::UBTT_Focus()
{
}

EBTNodeResult::Type UBTT_Focus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	// AActor* FocusActor = UBTFunctionLibrary::GetBlackboardValueAsActor(this,FocusTarget);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	
	UObject* Temp = BlackboardComp->GetValueAsObject(FocusTarget.SelectedKeyName);
	AActor* FocusActor = Cast<AActor>(Temp);
	AAIController* OwnerController = OwnerComp.GetAIOwner();
	
	if(FocusActor)
	{
		OwnerController->SetFocus(FocusActor);
	}
	else
	{
		FVector FocusLocation =BlackboardComp->GetValueAsVector(FocusTarget.SelectedKeyName);
			// UBTFunctionLibrary::GetBlackboardValueAsVector(this,FocusTarget);
		if(FAISystem::IsValidLocation(FocusLocation))
		{
			OwnerController->SetFocalPoint(FocusLocation);			
		}
	}
	
	return EBTNodeResult::Succeeded;	
}
