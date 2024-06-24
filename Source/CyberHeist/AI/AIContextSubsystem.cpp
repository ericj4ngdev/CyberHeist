// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIContextSubsystem.h"
#include "AIController.h"
#include "Engine/World.h"

UAIContextSubsystem::UAIContextSubsystem()
{
}

UAIContextSubsystem* UAIContextSubsystem::Get(AAIController* Controller)
{
	if (Controller)
	{
		if (UWorld* World = Controller->GetWorld())
		{
			return World->GetSubsystem<UAIContextSubsystem>();
		}
	}
	return nullptr;
}

void UAIContextSubsystem::RegisterPathingMoveGoal(AAIController* Controller, const FVector& Goal)
{
	if (Controller)
	{
		AIPathingGoals.Add(Controller, Goal);
	}
}

void UAIContextSubsystem::UnregisterPathingMoveGoal(AAIController* Controller)
{
	if (Controller)
	{
		AIPathingGoals.Remove(Controller);
	}
}

TArray<FVector> UAIContextSubsystem::GetMovementGoalsWithinRadius(const FVector& Origin, float Radius) const
{
	TArray<FVector> GoalsWithinRadius;
	for (const auto& Entry : AIPathingGoals)
	{
		const FVector& Goal = Entry.Value;
		if (FVector::Dist(Goal, Origin) <= Radius)
		{
			GoalsWithinRadius.Add(Goal);
		}
	}
	return GoalsWithinRadius;
}
