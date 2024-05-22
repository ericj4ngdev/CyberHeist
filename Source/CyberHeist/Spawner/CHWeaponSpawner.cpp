// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner/CHWeaponSpawner.h"

#include "Weapon/Gun/CHGunBase.h"

ACHWeaponSpawner::ACHWeaponSpawner()
{
	// PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ACHWeaponSpawner::BeginPlay()
{
	Super::BeginPlay();	
	if (HasAuthority())
	{
		SpawnGun();
	}
}

void ACHWeaponSpawner::SpawnGun()
{
	SpawnedGun = GetWorld()->SpawnActor<ACHGunBase>(GunBaseClass, GetActorTransform());
}