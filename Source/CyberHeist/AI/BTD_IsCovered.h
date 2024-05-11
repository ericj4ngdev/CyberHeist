// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_IsCovered.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UBTD_IsCovered : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTD_IsCovered();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
