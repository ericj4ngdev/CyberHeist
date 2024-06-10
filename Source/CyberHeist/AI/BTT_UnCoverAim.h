// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_UnCoverAim.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class CYBERHEIST_API UBTT_UnCoverAim : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTT_UnCoverAim();
	void AddCoverFilter(FBlackboardKeySelector& NewBlackboardKey, UObject* Owner, FName PropertyName);

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
