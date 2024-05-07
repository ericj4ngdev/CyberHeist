// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "BTS_StopAttackingIfTargetIsDead.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UBTS_StopAttackingIfTargetIsDead : public UBTService_BlueprintBase
{
	GENERATED_BODY()
	
public:
	UBTS_StopAttackingIfTargetIsDead();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector AttackTargetKey;
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
