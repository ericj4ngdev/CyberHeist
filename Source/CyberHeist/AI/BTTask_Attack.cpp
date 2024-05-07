// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Attack.h"
#include "AIController.h"
#include "CHAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/CHCharacterAIInterface.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = "BTT_Attack";
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	
	UObject* Temp = BlackboardComp->GetValueAsObject(AttackTarget.SelectedKeyName);
	// TargetActor is Set
	AActor* AttackActor = Cast<AActor>(Temp);
	
	
	ICHCharacterAIInterface* AIPawn = Cast<ICHCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	FAICharacterAttackFinished OnAttackFinished;
	OnAttackFinished.BindLambda(
		[&]()
		{
			// UE_LOG(LogTemp, Log, TEXT("BindLambda"));
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);			
		}
	);

	AIPawn->SetAIAttackDelegate(OnAttackFinished);	// npc에게 델리 전달.	
	AIPawn->AttackByAI(AttackActor);			// 공격 후 막줄에 NotifyComboActionEnd로

	// 3초 후에 ISATTACKING = false로 만들고 공격 멈추기
	OwnerComp.GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, &OwnerComp]()
	{
		// OwnerComp.GetBlackboardComponent()->SetValueAsBool(BBKEY_ISATTACKING, false);
		
		UE_LOG(LogTemp,Log,TEXT("Stop"));
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}, Duration, false);
	
	return EBTNodeResult::InProgress;
}
