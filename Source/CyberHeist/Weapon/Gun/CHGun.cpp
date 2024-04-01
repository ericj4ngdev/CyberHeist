// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/CHGun.h"

#include "CHProjectile.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/CHAnimInstance.h"
#include "Character/CHCharacterBase.h"
#include "Character/CHCharacterPlayer.h"
#include "KisMet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/DamageEvents.h"
#include "Components/CapsuleComponent.h"

#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"

class UEnhancedInputLocalPlayerSubsystem;
// Sets default values
ACHGun::ACHGun()
{
	PrimaryActorTick.bCanEverTick = false;
	
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
	WeaponMesh->CastShadow = true;
	WeaponMesh->SetVisibility(true, true);
	WeaponMesh->SetupAttachment(CollisionComp);	

	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));
	Effect->SetupAttachment(CollisionComp);
	
	/*static ConstructorHelpers::FObjectFinder<UParticleSystem> MuzzleFlashRef(TEXT("/Script/Engine.ParticleSystem'/Game/AssetPacks/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_MuzzleFlash.P_Wraith_Primary_MuzzleFlash'"));
	if (MuzzleFlashRef.Object) 
	{
		Effect->SetTemplate(MuzzleFlashRef.Object);
	}*/

	Effect->bAutoActivate = false;
	/*static ConstructorHelpers::FObjectFinder<UParticleSystem> ImpactRef(TEXT("/Script/Engine.ParticleSystem'/Game/AssetPacks/ShooterGame/Effects/ParticleSystems/Weapons/AssaultRifle/Impacts/P_AssaultRifle_IH.P_AssaultRifle_IH'"));
	if (ImpactRef.Object)
	{
		ImpactEffect = ImpactRef.Object;
	}*/

	MaxRange = 5000;
	Damage = 10;

	DefaultShootingType = EWeaponShootType::LineTrace;
	// ShootingType = DefaultShootingType;

	DefaultFireMode = EFireMode::SemiAutomatic;
	// FireMode = DefaultFireMode;
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
	PickUpOnTouch(Cast<ACHCharacterBase>(Other));	
}

// Call by CharacterPlayer
void ACHGun::Equip()
{	
	bIsEquipped = true;
	
	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(OwningCharacter->GetMesh(), AttachmentRules, FName(TEXT("Weapon_rSocket")));
	WeaponMesh->SetVisibility(true, true);
	WeaponMesh->CastShadow = true;
	
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
			if(ShootingType == EWeaponShootType::LineTrace)
			{
				EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ACHGun::PullTriggerLine);				
			}
			else if(ShootingType == EWeaponShootType::Projectile)
			{
				EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ACHGun::PullTriggerProjectile);				
			}
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Canceled, this, &ACHGun::CancelPullTrigger);

			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ACHGun::StartAim);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Canceled, this, &ACHGun::StopAim);
		}
	}
}

void ACHGun::UnEquip()
{
	if (OwningCharacter == nullptr)
	{
		return;
	}

	bIsEquipped = false;
	
	// WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh->SetVisibility(false, false);
	WeaponMesh->CastShadow = false;
	
	if (APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

void ACHGun::SetOwningCharacter(ACHCharacterBase* InOwningCharacter)
{
	OwningCharacter = InOwningCharacter;
	if (OwningCharacter == nullptr)	return;
	SetOwner(OwningCharacter);
}

void ACHGun::PickUpOnTouch(ACHCharacterBase* InCharacter)
{
	// bIsEquipped = true;
	InCharacter->AddWeaponToInventory(this,true);
}

void ACHGun::FireProjectile()
{
	if (OwningCharacter == nullptr || OwningCharacter->GetController() == nullptr)
	{
		return;
	}
	
	/*if (MuzzleFlash != nullptr)
	{
		AActor* ActorOwner = GetOwner();
		if (ActorOwner != nullptr)
		{
			UParticleSystemComponent* ParticleComponent = UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, ActorOwner->GetRootComponent(), TEXT("MuzzleFlashSocket"));
			if (ParticleComponent != nullptr)
			{
				ParticleComponent->Activate(true);
				float Duration = 0.1f; 
				FTimerHandle TimerHandle;
				FTimerManager& TimerManager = GetWorld()->GetTimerManager();
				TimerManager.SetTimer(TimerHandle, [ParticleComponent]()
					{
						if (ParticleComponent->IsValidLowLevel())
						{
							ParticleComponent->DeactivateSystem();
						}
					}, Duration, false);
			}
		}
	}	*/

	// Try and play effect
	Effect->Activate(true);
	float Duration = 0.1f; // Set the duration time in seconds
	GetWorldTimerManager().SetTimer(DurationTimerHandle, this, &ACHGun::StopParticleSystem, Duration, false);

	
	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;			

			ActorSpawnParams.Owner = GetOwner();
			APawn* InstigatorPawn = Cast<APawn>(GetOwner());
			ActorSpawnParams.Instigator = InstigatorPawn;

			ACHProjectile* Projectile = World->SpawnActor<ACHProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			if(Projectile) Projectile->SetOwner(OwningCharacter);			// 
		}
	}

	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, OwningCharacter->GetActorLocation());
	}

	// Get the animation object for the arms mesh
	UAnimInstance* AnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr)
	{
		UCHAnimInstance* CHAnimInstance = Cast<UCHAnimInstance>(AnimInstance);
		if (CHAnimInstance)
		{
			UE_LOG(LogTemp, Log, TEXT("ReCoil"));
			CHAnimInstance->Recoil(10);
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

	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, OwningCharacter->GetActorLocation());
	}

	// Get the animation object for the arms mesh
	UAnimInstance* AnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr)
	{
		UCHAnimInstance* CHAnimInstance = Cast<UCHAnimInstance>(AnimInstance);
		if (CHAnimInstance)
		{
			UE_LOG(LogTemp, Log, TEXT("ReCoil"));
			CHAnimInstance->Recoil(10);
		}
	}	
}

void ACHGun::PullTriggerLine()
{	
	if(bIsEquipped)
	{
		OwningCharacter->bUseControllerRotationYaw = true;
		if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdAim
			|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim)
		{
			OwningCharacter->SetCombatMode(true);
			// GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHGun::FireLine, FireInterval, true);

			if(FireMode == EFireMode::Automatic)
			{
				GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHGun::FireLine, FireInterval, true);
			}
			if(FireMode == EFireMode::SemiAutomatic)
			{
				FireLine();					
			}
		}
		if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third ||
			OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First)
		{
			// hold a gun
			OwningCharacter->SetCombatMode(true);
			// UE_LOG(LogTemp, Log, TEXT("SetCombatMode true"));

			if(FireMode == EFireMode::Automatic)
			{
				// holding a gun delay
				GetWorld()->GetTimerManager().SetTimer(ShootTimerHandle, [this]()
				{
					// Fire();
					// Activate the timer to continuously fire at intervals
					GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHGun::FireLine, FireInterval, true);
				}, ShootingPreparationTime, false);	
			}
			if(FireMode == EFireMode::SemiAutomatic)
			{
				GetWorld()->GetTimerManager().SetTimer(ShootTimerHandle, [this]()
				{
					FireLine();
				}, ShootingPreparationTime, false);	
			}
		}
		bTrigger = true;
	}	
}

void ACHGun::PullTriggerProjectile()
{
	if(bIsEquipped)
	{
		OwningCharacter->bUseControllerRotationYaw = true;
		if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdAim
			|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim)
		{
			OwningCharacter->SetCombatMode(true);
			// GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHGun::FireProjectile, FireInterval, true);
			if(FireMode == EFireMode::Automatic)
			{
				GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHGun::FireProjectile, FireInterval, true);
			}
			if(FireMode == EFireMode::SemiAutomatic)
			{
				FireProjectile();
			}
		}
		if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third ||
			OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First)
		{
			// hold a gun
			OwningCharacter->SetCombatMode(true);

			if(FireMode == EFireMode::Automatic)
			{
				// holding a gun delay
				GetWorld()->GetTimerManager().SetTimer(ShootTimerHandle, [this]()
				{
					GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHGun::FireProjectile, FireInterval, true);
				}, ShootingPreparationTime, false);	
			}
			if(FireMode == EFireMode::SemiAutomatic)
			{			
				GetWorld()->GetTimerManager().SetTimer(ShootTimerHandle, [this]()
				{
					FireProjectile();
				}, ShootingPreparationTime, false);	
			}
		}
		bTrigger = true;
	}
}

void ACHGun::CancelPullTrigger()
{
	if(bIsEquipped)
	{
		ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);
		if (PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::Third)
			PlayerCharacter->bUseControllerRotationYaw = false; 

		if (PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::Third ||
			PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::First)
		{
			// Cancel holding a gun
			OwningCharacter->SetCombatMode(false);
		
			GetWorld()->GetTimerManager().ClearTimer(ShootTimerHandle);
		}
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
		bTrigger = false;
	}
}

void ACHGun::StartAim()
{
	if(bIsEquipped)
	{
		ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);
		if (PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::Third)
		{
			PlayerCharacter->SetCharacterControl(ECharacterControlType::ThirdAim);
		}
		if (PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::First)
		{
			PlayerCharacter->SetCharacterControl(ECharacterControlType::FirstAim);
		}
		// if Pull Triggering, pass
		if(!bTrigger) OwningCharacter->SetCombatMode(true);
	}
}

void ACHGun::StopAim()
{
	if(bIsEquipped)
	{
		ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);
		if (PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdAim)
		{
			PlayerCharacter->SetCharacterControl(ECharacterControlType::Third);
		
		}
		if (PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim)
		{
			PlayerCharacter->SetCharacterControl(ECharacterControlType::First);
		
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
}

void ACHGun::StopParticleSystem()
{
	// UE_LOG(LogTemp, Warning, TEXT("StopParticleSystem"));

	// Deactivate the ParticleSystemComponent to stop playing the particle effect
	Effect->Deactivate();
}
