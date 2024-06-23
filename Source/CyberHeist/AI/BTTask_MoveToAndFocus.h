// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_MoveToAndFocus.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UBTTask_MoveToAndFocus : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	UBTTask_MoveToAndFocus();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector FocusTarget;
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
