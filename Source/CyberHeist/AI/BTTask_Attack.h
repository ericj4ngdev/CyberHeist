// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Attack.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Attack();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector AttackTarget;
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
private:
	FTimerHandle StopTimerHandle;
	FTimerHandle TimerHandle;
public:
	UPROPERTY(EditAnywhere, Category = "Timer")
	float AttackInterval;

	UPROPERTY(EditAnywhere, Category = "Timer")
	float Duration;
};
