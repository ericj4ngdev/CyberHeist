// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_FindPatrolPos.h"
#include "CHAI.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/CHCharacterAIInterface.h"

UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	// UE_LOG(LogTemp, Log, TEXT("ExecuteTask"));
	
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		// UE_LOG(LogTemp, Log, TEXT("ControllingPawn is null"));
		return EBTNodeResult::Failed;
	}

	// 월드값은 폰이 위치한 월드를 가져와 넣는다. 
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
	if (nullptr == NavSystem)
	{
		// UE_LOG(LogTemp, Log, TEXT("NavSystem is null"));
		return EBTNodeResult::Failed;
	}
	
	// ControllingPawn 가 인터페이스를 가지고 있는지 확인
	ICHCharacterAIInterface* AIPawn = Cast<ICHCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		UE_LOG(LogTemp, Log, TEXT("AIPawn is null"));
		return EBTNodeResult::Failed;
	}
	
	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKEY_HOMEPOS);
	// UE_LOG(LogTemp, Log, TEXT("ExecuteTask"));
	float PatrolRadius = AIPawn->GetAIPatrolRadius();
	FNavLocation NextPatrolPos;
	
	// 반경 내에 랜덤한 위치 지정
	if (NavSystem->GetRandomPointInNavigableRadius(Origin, PatrolRadius, NextPatrolPos))
	{
		UE_LOG(LogTemp, Log, TEXT("Succeeded"));
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(BBKEY_PATROLPOS, NextPatrolPos.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
