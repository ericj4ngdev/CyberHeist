// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/CHGameState.h"
#include "CyberHeist.h"

void ACHGameState::HandleBeginPlay()
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"));
	
	Super::HandleBeginPlay();

	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"));
}

void ACHGameState::OnRep_ReplicatedHasBegunPlay()
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"));
	Super::OnRep_ReplicatedHasBegunPlay();
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"));
}
