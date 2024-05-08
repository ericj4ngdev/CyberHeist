// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "CHAI.h"
#include "BTT_SetMovementSpeed.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UBTT_SetMovementSpeed : public UBTTask_BlueprintBase
{
	GENERATED_BODY()

public:
	UBTT_SetMovementSpeed();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECHAIMovementSpeed MovementSpeed;
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
