// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CHWeaponSpawner.generated.h"

class ACHGunBase;

UCLASS()
class CYBERHEIST_API ACHWeaponSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ACHWeaponSpawner();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	ACHGunBase* SpawnedGun;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACHGunBase> GunBaseClass;

public:
	void SpawnGun();

};
