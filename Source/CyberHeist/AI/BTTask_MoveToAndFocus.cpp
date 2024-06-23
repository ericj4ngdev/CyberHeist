// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_MoveToAndFocus.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Interface/CHCharacterAIInterface.h"

UBTTask_MoveToAndFocus::UBTTask_MoveToAndFocus()
{
	NodeName = "BTTask_MoveToAndFocus";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_MoveToAndFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type NodeResult = Super::ExecuteTask(OwnerComp, NodeMemory);
	// Super::ExecuteTask(OwnerComp, NodeMemory);
	
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
	
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	
	UObject* Temp = BlackboardComp->GetValueAsObject(FocusTarget.SelectedKeyName);
	AActor* FocusActor = Cast<AActor>(Temp);
	AAIController* OwnerController = OwnerComp.GetAIOwner();
	
	if(FocusActor)
	{
		FRotator Lookat = (FocusActor->GetActorLocation() - ControllingPawn->GetActorLocation()).Rotation(); 
		// FVector Temp = (ControllingPawn->GetActorRotation().Vector() * 0.8 + Lookat.Vector() * 0.2 );
		// OwnerController->GetPawn()->SetActorRotation(Temp.Rotation());
		
		// FVector End = ControllingPawn->GetActorLocation() + (FocusActor->GetActorLocation() - ControllingPawn->GetActorLocation()) * 20; 
		// DrawDebugDirectionalArrow(GetWorld(), ControllingPawn->GetActorLocation(), End,5.0f, FColor::Yellow, false, 1, 0, 1.0f);
		UE_LOG(LogTemp, Log, TEXT("FocusActor : %s "), *FocusActor->GetActorLocation().ToString());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("FocusActor is null"))
	}
	return NodeResult;
}

void UBTTask_MoveToAndFocus::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	UObject* Temp = BlackboardComp->GetValueAsObject(FocusTarget.SelectedKeyName);
	AActor* FocusActor = Cast<AActor>(Temp);
	AAIController* OwnerController = OwnerComp.GetAIOwner();

	if (FocusActor)
	{
		// FRotator Lookat = (FocusActor->GetActorLocation() - ControllingPawn->GetActorLocation()).Rotation();
		//FVector Temp = (ControllingPawn->GetActorRotation().Vector() * 0.8 + Lookat.Vector() * 0.2 );
		// OwnerController->GetPawn()->SetActorRotation(Temp.Rotation());
		// ControllingPawn->SetActorRotation(Lookat);
	}
}
