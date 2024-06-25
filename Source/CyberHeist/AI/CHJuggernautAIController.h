// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/CHAIControllerBase.h"
#include "CHJuggernautAIController.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API ACHJuggernautAIController : public ACHAIControllerBase
{
	GENERATED_BODY()

public:
	ACHJuggernautAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void RunAI()override;
	virtual void StopAI()override;

	virtual void OnPossess(APawn* InPawn) override;

public:
	UFUNCTION()
	void HandleSightSense(AActor* Actor, FAIStimulus Stimulus);
};
