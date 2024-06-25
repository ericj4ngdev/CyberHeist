// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CHGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTotalKilledMonsterCountChangedDelegate, int32);
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
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	FORCEINLINE const int32& GetTotalKilledMonsterCount() const { return TotalKilledMonsterCount; }
	FORCEINLINE void SetTotalKilledMonsterCount(int32 NewTotalKilledMonsterCount) { TotalKilledMonsterCount = NewTotalKilledMonsterCount; }

	FOnTotalKilledMonsterCountChangedDelegate OnTotalKilledMonsterCountChangedDelegate;
protected:
	UFUNCTION()
	virtual void OnRep_TotalKilledMonsterCount();

	UPROPERTY(ReplicatedUsing = OnRep_TotalKilledMonsterCount)
	int32 TotalKilledMonsterCount = 0;
};
