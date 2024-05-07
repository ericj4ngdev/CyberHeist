// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Focus.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UBTT_Focus : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_Focus();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector FocusTarget;
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
