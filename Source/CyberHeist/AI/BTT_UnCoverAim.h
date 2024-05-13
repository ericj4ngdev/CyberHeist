// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_UnCoverAim.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UBTT_UnCoverAim : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_UnCoverAim();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector CoverKey;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
