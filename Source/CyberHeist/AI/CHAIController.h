// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "CHAIController.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API ACHAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACHAIController();

public:
	void RunAI();
	void StopAI();

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBlackboardData> BBAsset;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBehaviorTree> BTAsset;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UAIPerceptionComponent> AIPerception;

	UFUNCTION()
	void HandleSightSense(AActor* Actor, FAIStimulus Stimulus);
	
};
