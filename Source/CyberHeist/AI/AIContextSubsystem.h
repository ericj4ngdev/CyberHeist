// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Subsystems/WorldSubsystem.h"
#include "AIContextSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UAIContextSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UAIContextSubsystem();

	static UAIContextSubsystem* Get(AAIController* Controller);

	void RegisterPathingMoveGoal(AAIController* Controller, const FVector& Goal);
	void UnregisterPathingMoveGoal(AAIController* Controller);

	// Get all movement goals within a certain radius
	TArray<FVector> GetMovementGoalsWithinRadius(const FVector& Origin, float Radius) const;

private:
	TMap<AAIController*, FVector> AIPathingGoals;
};
