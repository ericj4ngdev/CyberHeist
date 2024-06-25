// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/CHAIControllerBase.h"
#include "Character/CHCharacterPlayer.h"
#include "CHCloneAIController.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API ACHCloneAIController : public ACHAIControllerBase
{
	GENERATED_BODY()

public:
	ACHCloneAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void RunAI()override;
	virtual void StopAI()override;

	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void HandleSightSenseArray(const TArray<AActor*>& Actors);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TArray<ACHCharacterPlayer*> RecognizedPlayers; // Recognized players array
	
};
