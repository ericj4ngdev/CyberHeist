// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner/CHEnemySpawner.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Character/CHCharacterNonPlayer.h"

// Sets default values
ACHEnemySpawner::ACHEnemySpawner()
{
	// PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

}

// Called when the game starts or when spawned
void ACHEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		// SpawnEnemy();
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ACHEnemySpawner::SpawnEnemy, 10.0f, false);
	}
}

void ACHEnemySpawner::SpawnEnemy()
{
	SpawnedEnemy = GetWorld()->SpawnActor<ACHCharacterNonPlayer>(EnemyType, GetActorTransform());
}
