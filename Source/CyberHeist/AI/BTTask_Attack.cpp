// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Attack.h"
#include "AIController.h"
#include "CHAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/CHCharacterAIInterface.h"

UBTTask_Attack::UBTTask_Attack()
{
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGETACTOR));
	if(TargetPawn == nullptr) return EBTNodeResult::Failed;
	
	// TargetActor is Set
	// UObject* Target = OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGETACTOR);
	// APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGETACTOR));
	if(TargetPawn != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Target : %s"), *TargetPawn->GetName());
		AIPawn->AttackByAI();
		return EBTNodeResult::InProgress;
	}
	// else return EBTNodeResult::Failed;
	
	AIPawn->CancelAttackByAI();
	return EBTNodeResult::Succeeded;
}
