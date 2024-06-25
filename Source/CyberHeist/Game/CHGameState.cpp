// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/CHGameState.h"
#include "CyberHeist.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

void ACHGameState::HandleBeginPlay()
{
	// CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"))	
	Super::HandleBeginPlay();
	// CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"))
}

void ACHGameState::OnRep_ReplicatedHasBegunPlay()
{
	// CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"))
	Super::OnRep_ReplicatedHasBegunPlay();
	// CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"))
}

void ACHGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACHGameState, TotalKilledEnemyCount);
}

void ACHGameState::OnRep_TotalKilledEnemyCount()
{
	OnTotalKilledEnemyCountChangedDelegate.Broadcast(TotalKilledEnemyCount);
}
