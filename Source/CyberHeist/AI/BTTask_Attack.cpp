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

	APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGETACTOR));
	if(nullptr == TargetPawn)
	{
		return EBTNodeResult::Failed;
	}
	
	ICHCharacterAIInterface* AIPawn = Cast<ICHCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return EBTNodeResult::Failed;
	}
	// TargetActor is Set
	// UE_LOG(LogTemp, Log, TEXT("Target : %s"), *TargetPawn->GetName());

	FAICharacterAttackFinished OnAttackFinished;
	OnAttackFinished.BindLambda(
		[&]()
		{
			UE_LOG(LogTemp, Log, TEXT("BindLambda"));
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	);

	AIPawn->SetAIAttackDelegate(OnAttackFinished);	// npc에게 델리 전달.
	// NotifyComboActionEnd?
	AIPawn->AttackByAI();			// 공격 후 막줄에 NotifyComboActionEnd로
	// OnAttackFinished.ExecuteIfBound(); 가 npc에서 호출 -> 람다가 호출
	// 그런데 교수님과 다른건 공격 end부분이 없다. 
	// return EBTNodeResult::Succeeded;
	return EBTNodeResult::InProgress;
}
