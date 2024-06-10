// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner/CHWeaponSpawner.h"

#include "CyberHeist.h"
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
	CH_LOG(LogCHNetwork, Log, TEXT("Begin"))
	FActorSpawnParameters Param;
	Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
	
	SpawnedGun = GetWorld()->SpawnActor<ACHGunBase>(GunBaseClass, GetActorTransform(), Param);
	CH_LOG(LogCHNetwork, Log, TEXT("End"))
}