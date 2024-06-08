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
	SpawnTimer = 5.0f;
}

// Called when the game starts or when spawned
void ACHEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	/*if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ACHEnemySpawner::SpawnEnemyWithWeapon, SpawnTimer, false);
	}*/
}

void ACHEnemySpawner::SpawnEnemyWithWeapon()
{
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ACHEnemySpawner::SpawnEnemy, 0.1f, false);
	SpawnGun();
}

void ACHEnemySpawner::SpawnEnemy()
{
	SpawnedEnemy = GetWorld()->SpawnActor<ACHCharacterNonPlayer>(EnemyType, GetActorTransform());
}

void ACHEnemySpawner::SpawnGun()
{
	SpawnedGun = GetWorld()->SpawnActor<ACHGunBase>(GunBaseClass, GetActorTransform());
}
