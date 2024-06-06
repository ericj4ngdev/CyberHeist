// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTS_UpdateDistanceToTarget.h"
#include "CHAI.h"
#include "AIController.h"
#include "CHAIController.h"
#include "Interface/CHCharacterAIInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Character/CHCharacterBase.h"

UBTS_UpdateDistanceToTarget::UBTS_UpdateDistanceToTarget()
{
	Interval = 0.5f;
}

void UBTS_UpdateDistanceToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(__FUNCTION__))
	
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return;
	}

	UWorld* World = ControllingPawn->GetWorld();
	if (nullptr == World)
	{
		return;
	}

	ICHCharacterAIInterface* AIPawn = Cast<ICHCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return;
	}
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	
	UObject* Temp = BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName);
	// 플레이어가 죽어서 없는 경우 
	if(Temp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s LockedActor is null"), *FString(__FUNCTION__))
		return;
	}
	
	AActor* Target = Cast<AActor>(Temp);
	float Distance = FVector::Distance(Target->GetActorLocation(),ControllingPawn->GetActorLocation());
	UE_LOG(LogTemp,Log,TEXT("Distance : %f"), Distance)
	
	BlackboardComp->SetValueAsFloat(DistanceToTargetKey.SelectedKeyName,Distance);
}
