// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/CHGun.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/CHCharacterPlayer.h"
#include "KisMet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/DamageEvents.h"
#include "Components/CapsuleComponent.h"

#include "Character/CHCharacterBase.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"

class UEnhancedInputLocalPlayerSubsystem;
// Sets default values
ACHGun::ACHGun()
{
	PrimaryActorTick.bCanEverTick = false;

	bSpawnWithCollision = true;
	
	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root"));
	CollisionComp->InitCapsuleSize(40.0f, 50.0f);
	CollisionComp->SetCollisionObjectType(ECC_GameTraceChannel1);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Manually enable when in pickup mode
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = CollisionComp;
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetupAttachment(CollisionComp);
	WeaponMesh->SetRelativeLocation(WeaponMeshPickupRelativeLocation);
	WeaponMesh->CastShadow = true;
	WeaponMesh->SetVisibility(true, true);
	WeaponMesh->SetupAttachment(CollisionComp);	

	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));
	Effect->SetupAttachment(CollisionComp);
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem> MuzzleFlashRef(TEXT("/Script/Engine.ParticleSystem'/Game/AssetPacks/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_MuzzleFlash.P_Wraith_Primary_MuzzleFlash'"));
	if (MuzzleFlashRef.Object) 
	{
		Effect->SetTemplate(MuzzleFlashRef.Object);
		Effect->bAutoActivate = false;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ImpactRef(TEXT("/Script/Engine.ParticleSystem'/Game/AssetPacks/ShooterGame/Effects/ParticleSystems/Weapons/AssaultRifle/Impacts/P_AssaultRifle_IH.P_AssaultRifle_IH'"));
	if (ImpactRef.Object)
	{
		ImpactEffect = ImpactRef.Object;
	}
}

// Called when the game starts or when spawned
void ACHGun::BeginPlay()
{
	// Attach the ParticleSystemComponent to the MuzzleFlashSocket
	Effect->AttachToComponent(WeaponMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("MuzzleFlashSocket"));
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	
	Super::BeginPlay();
}


void ACHGun::NotifyActorBeginOverlap(AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);
	
	AttachWeapon(Cast<ACHCharacterPlayer>(Other));	
}

// Call by CharacterPlayer
void ACHGun::AttachWeapon(ACHCharacterPlayer* TargetCharacter)
{	
	OwningCharacter = TargetCharacter;
	if (OwningCharacter == nullptr)	return;
	SetOwner(OwningCharacter);
	
	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(OwningCharacter->GetMesh(), AttachmentRules, FName(TEXT("Weapon_rSocket")));
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// switch bHasRifle so the animation blueprint can switch to another animation set
	OwningCharacter->SetHasRifle(true);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ACHGun::PullTrigger);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Canceled, this, &ACHGun::CancelPullTrigger);

			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ACHGun::StartAim);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Canceled, this, &ACHGun::StopAim);
		}
	}
}


void ACHGun::FireLine()
{
	// UE_LOG(LogTemp, Warning, TEXT("PullTrigger"));
	// UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh, TEXT("MuzzleFlashSocket"));
	Effect->Activate(true);
	float Duration = 0.1f; // Set the duration time in seconds
	GetWorldTimerManager().SetTimer(DurationTimerHandle, this, &ACHGun::StopParticleSystem, Duration, false);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerPawn"));
		return;
	}
	AController* OwnerController = OwnerPawn->GetController();
	ensure(OwnerController);
	if (OwnerController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerController"));
		return;
	}

	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);

	// DrawDebugCamera(GetWorld(), Location, Rotation, 90, 2, FColor::Red, true);
	
	FVector End = Location + Rotation.Vector() * MaxRange;

	// LineTrace
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1, Params);
	if (bSuccess)
	{
		FVector ShotDirection = -Rotation.Vector();
		DrawDebugPoint(GetWorld(), Hit.Location, 20, FColor::Red, true);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ImpactEffect, Hit.Location, ShotDirection.Rotation());

		AActor* HitActor = Hit.GetActor();
		if (HitActor != nullptr)
		{
			FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
			HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
		}
	}
}


void ACHGun::PullTrigger()
{
	OwningCharacter->bUseControllerRotationYaw = true;
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdAim
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim)
	{
		OwningCharacter->SetCombatMode(true);
		GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHGun::FireLine, FireInterval, true);
	}
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third ||
		OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First)
	{
		// hold a gun
		OwningCharacter->SetCombatMode(true);
		// UE_LOG(LogTemp, Log, TEXT("SetCombatMode true"));
		// holding a gun delay
		GetWorld()->GetTimerManager().SetTimer(ShootTimerHandle, [this]()
		{
			// Fire();
			// Activate the timer to continuously fire at intervals
			GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHGun::FireLine, FireInterval, true);
		}, ShootingPreparationTime, false);		
	}

	bTrigger = true;
}

void ACHGun::CancelPullTrigger()
{
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third)
		OwningCharacter->bUseControllerRotationYaw = false; 

	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third ||
		OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First)
	{
		// Cancel holding a gun
		OwningCharacter->SetCombatMode(false);
		
		GetWorld()->GetTimerManager().ClearTimer(ShootTimerHandle);
	}
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	bTrigger = false;
}

void ACHGun::StartAim()
{
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third)
	{
		OwningCharacter->SetCharacterControl(ECharacterControlType::ThirdAim);
	}
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First)
	{
		OwningCharacter->SetCharacterControl(ECharacterControlType::FirstAim);
	}
	// if PullTriggering, pass
	if(!bTrigger) OwningCharacter->SetCombatMode(true);
}

void ACHGun::StopAim()
{
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdAim)
	{
		OwningCharacter->SetCharacterControl(ECharacterControlType::Third);
		
	}
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim)
	{
		OwningCharacter->SetCharacterControl(ECharacterControlType::First);
		
	}
	if(!bTrigger)
	{
		OwningCharacter->SetCombatMode(false); // if PullTriggering, pass
	}
	else
	{
		OwningCharacter->bUseControllerRotationYaw = true;		
	}
}

void ACHGun::StopParticleSystem()
{
	// UE_LOG(LogTemp, Warning, TEXT("StopParticleSystem"));

	// Deactivate the ParticleSystemComponent to stop playing the particle effect
	Effect->Deactivate();
}
