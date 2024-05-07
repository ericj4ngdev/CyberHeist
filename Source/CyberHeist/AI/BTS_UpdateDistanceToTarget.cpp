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
}

void UBTS_UpdateDistanceToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

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
	AActor* Target = Cast<AActor>(Temp);
	
	// AActor* Target = UBTFunctionLibrary::GetBlackboardValueAsActor(this,AttackTargetKey);

	float Distance = FVector::Distance(Target->GetActorLocation(),AIOwner->GetPawn()->GetActorLocation());
	BlackboardComp->SetValueAsFloat(DistanceToTargetKey.SelectedKeyName,Distance);
	// UBTFunctionLibrary::SetBlackboardValueAsFloat(this,DistanceToTargetKey,Distance);	
}
