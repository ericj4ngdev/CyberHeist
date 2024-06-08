// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CHCharacterNonPlayer.h"
#include "GameFramework/Actor.h"
#include "CHEnemySpawner.generated.h"

UCLASS()
class CYBERHEIST_API ACHEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACHEnemySpawner();

protected:
	virtual void BeginPlay() override;

private:
	// Timer handle for delayed spawn
	FTimerHandle SpawnTimerHandle;

	UPROPERTY(EditAnywhere)
	float SpawnTimer;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACHCharacterNonPlayer> EnemyType;


	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACHGunBase> GunBaseClass;

public:
	UPROPERTY()
	ACHGunBase* SpawnedGun;
	
	UPROPERTY()
	ACHCharacterNonPlayer* SpawnedEnemy;
	
	void SpawnEnemyWithWeapon();
	void SpawnEnemy();
	void SpawnGun();

};
