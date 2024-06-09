// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CHGameState.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API ACHGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void HandleBeginPlay() override;

	virtual void OnRep_ReplicatedHasBegunPlay() override;
};
