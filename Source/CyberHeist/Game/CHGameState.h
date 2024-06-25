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
	FORCEINLINE const int32& GetTotalKilledEnemyCount() const { return TotalKilledEnemyCount; }
	FORCEINLINE void SetTotalKilledEnemyCount(int32 NewTotalKilledEnemyCount) { TotalKilledEnemyCount = NewTotalKilledEnemyCount; }

	FOnTotalKilledMonsterCountChangedDelegate OnTotalKilledEnemyCountChangedDelegate;
protected:
	UFUNCTION()
	virtual void OnRep_TotalKilledEnemyCount();

	UPROPERTY(ReplicatedUsing = OnRep_TotalKilledEnemyCount)
	int32 TotalKilledEnemyCount = 0;
};
