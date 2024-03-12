// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/CHGun.h"
#include "KisMet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ACHGun::ACHGun()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);	

	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));
	Effect->SetupAttachment(Root);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> GunMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/AssetPacks/ShooterGame/Weapons/Rifle.Rifle'"));
	if (GunMeshRef.Object) Mesh->SetSkeletalMesh(GunMeshRef.Object);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> MuzzleFlashRef(TEXT("/Script/Engine.ParticleSystem'/Game/AssetPacks/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_MuzzleFlash.P_Wraith_Primary_MuzzleFlash'"));
	if (MuzzleFlashRef.Object) 
	{
		Effect->SetTemplate(MuzzleFlashRef.Object);
		Effect->bAutoActivate = false;
	}


}

// Called when the game starts or when spawned
void ACHGun::BeginPlay()
{
	Super::BeginPlay();
	// Attach the ParticleSystemComponent to the MuzzleFlashSocket
	Effect->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("MuzzleFlashSocket"));
}

// Called every frame
void ACHGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACHGun::PullTrigger()
{
	UE_LOG(LogTemp, Warning, TEXT("PullTrigger"));
	// UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh, TEXT("MuzzleFlashSocket"));
	Effect->Activate(true);
	float Duration = 0.1f; // Set the duration time in seconds
	GetWorldTimerManager().SetTimer(DurationTimerHandle, this, &ACHGun::StopParticleSystem, Duration, false);
}

void ACHGun::StopParticleSystem()
{
	UE_LOG(LogTemp, Warning, TEXT("StopParticleSystem"));

	// Deactivate the ParticleSystemComponent to stop playing the particle effect
	Effect->Deactivate();
}