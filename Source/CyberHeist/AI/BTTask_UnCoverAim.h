// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_UnCoverAim.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UBTTask_UnCoverAim : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_UnCoverAim();

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
