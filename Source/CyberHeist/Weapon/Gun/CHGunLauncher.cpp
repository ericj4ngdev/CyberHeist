// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/CHGunLauncher.h"

ACHGunLauncher::ACHGunLauncher() 
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> GunMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/AssetPacks/ShooterGame/Weapons/Launcher.Launcher'"));
	if (GunMeshRef.Object) Mesh->SetSkeletalMesh(GunMeshRef.Object);
}