// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CHPlayerState.generated.h"


USTRUCT()
struct FPlayStatistics
{
	GENERATED_BODY()

	UPROPERTY()
	int32 KilledEnemyCount = 12;
};

/**
 * 
 */
UCLASS()
class CYBERHEIST_API ACHPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ACHPlayerState();

	FORCEINLINE const int32& GetKilledEnemyCount() const { return PlayStatistics.KilledEnemyCount; }
	FORCEINLINE void AddKilledEnemyCount(int32 Count) { PlayStatistics.KilledEnemyCount = PlayStatistics.KilledEnemyCount + Count; }

	FORCEINLINE void SetPlayStatistics(const FPlayStatistics& NewPlayStatistics) { PlayStatistics = NewPlayStatistics; }
	FORCEINLINE const FPlayStatistics& GetPlayStatistics() const { return PlayStatistics; }
protected:
	FPlayStatistics PlayStatistics;
};
