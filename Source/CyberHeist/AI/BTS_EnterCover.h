// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "BTS_EnterCover.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UBTS_EnterCover : public UBTService_BlueprintBase
{
	GENERATED_BODY()

public:
	UBTS_EnterCover();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;	
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
