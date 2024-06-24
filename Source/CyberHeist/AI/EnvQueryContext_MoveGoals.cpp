// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnvQueryContext_MoveGoals.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIContextSubsystem.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"

void UEnvQueryContext_MoveGoals::ProvideContext(FEnvQueryInstance& QueryInstance,
                                                FEnvQueryContextData& ContextData) const
{
	UObject* QueryOwner = QueryInstance.Owner.Get();
	AAIController* AIController = Cast<AAIController>(QueryOwner);

	if (AIController)
	{
		UAIContextSubsystem* ContextSubsystem = UAIContextSubsystem::Get(AIController);
		if (ContextSubsystem)
		{
			TArray<FVector> MoveGoals = ContextSubsystem->GetMovementGoalsWithinRadius(AIController->GetPawn()->GetActorLocation(), 500.0f); // 예시로 500 유닛 반경 내 목표를 가져옴
			UEnvQueryItemType_Point::SetContextHelper(ContextData, MoveGoals);
		}
	}
}

