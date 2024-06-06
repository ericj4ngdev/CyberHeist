// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_UpdateDistanceToTarget.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UBTS_UpdateDistanceToTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTS_UpdateDistanceToTarget();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector AttackTargetKey;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector DistanceToTargetKey;
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
