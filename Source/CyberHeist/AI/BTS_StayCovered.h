// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BTService_OccupyCover.h"
#include "AI/Services/BTService_OccupyCover.h"
#include "BTS_StayCovered.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UBTS_StayCovered : public UBTService_OccupyCover
{
	GENERATED_BODY()

public:
	UBTS_StayCovered();

public:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
