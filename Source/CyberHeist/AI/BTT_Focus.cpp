// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_Focus.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Interface/CHCharacterAIInterface.h"

UBTT_Focus::UBTT_Focus()
{
	NodeName = "BTT_Focus";
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
		FRotator Lookat = (FocusActor->GetActorLocation() - ControllingPawn->GetActorLocation()).Rotation(); 
		OwnerController->GetPawn()->SetActorRotation(Lookat);
		// FVector End = ControllingPawn->GetActorLocation() + (FocusActor->GetActorLocation() - ControllingPawn->GetActorLocation()) * 20; 
		// DrawDebugDirectionalArrow(GetWorld(), ControllingPawn->GetActorLocation(), End,5.0f, FColor::Yellow, false, 1, 0, 1.0f);
		UE_LOG(LogTemp, Log, TEXT("FocusActor : %s "), *FocusActor->GetActorLocation().ToString());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("FocusActor is null"))
	}
	UE_LOG(LogTemp, Log, TEXT("UBTT_Focus::ExecuteTask"))
	return EBTNodeResult::Succeeded;	
}
