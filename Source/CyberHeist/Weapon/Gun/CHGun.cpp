// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/CHGun.h"

#include "AIController.h"
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
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // Manually enable when in pickup mode
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = CollisionComp;

	WeaponMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(FName("WeaponMesh1P"));
	WeaponMesh1P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponMesh1P->CastShadow = false;
	WeaponMesh1P->SetVisibility(false, true);
	WeaponMesh1P->SetupAttachment(CollisionComp);
	WeaponMesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	
	WeaponMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh3P"));
	WeaponMesh3P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponMesh3P->SetupAttachment(CollisionComp);
	// WeaponMesh3P->SetRelativeLocation(WeaponMesh3PickupRelativeLocation);
	WeaponMesh3P->CastShadow = true;
	WeaponMesh3P->SetVisibility(true, true);
	WeaponMesh3P->SetupAttachment(CollisionComp);
	WeaponMesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;

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
	Damage = 1;

	DefaultShootingType = EWeaponShootType::LineTrace;
	// ShootingType = DefaultShootingType;

	DefaultFireMode = EFireMode::SemiAutomatic;
	// FireMode = DefaultFireMode;
	
	bReloading = false;
	bInfiniteAmmo = false;
}

// Called when the game starts or when spawned
void ACHGun::BeginPlay()
{
	// Attach the ParticleSystemComponent to the MuzzleFlashSocket
	Effect->AttachToComponent(WeaponMesh3P, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("MuzzleFlashSocket"));
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	Super::BeginPlay();
}

UAnimMontage* ACHGun::GetEquip1PMontage() const
{
	return Equip1PMontage;
}

UAnimMontage* ACHGun::GetEquip3PMontage() const
{
	return Equip3PMontage;
}

void ACHGun::NotifyActorBeginOverlap(AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);
	 
	PickUpOnTouch(Cast<ACHCharacterBase>(Other));	
}

// Call by CharacterPlayer
void ACHGun::Equip()
{
	if (!OwningCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s %s OwningCharacter is nullptr"), *FString(__FUNCTION__), *GetName());
		return;
	}
	
	bIsEquipped = true;

	// FName AttachPoint = OwningCharacter->GetWeaponAttachPoint();
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

	if (WeaponMesh1P)
	{
		// FName(TEXT("WeaponPoint"))
		WeaponMesh1P->AttachToComponent(OwningCharacter->GetFirstPersonMesh(), AttachmentRules, AttachPoint1P);
		
		// WeaponMesh1P->AttachToComponent(OwningCharacter->GetFirstPersonMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, AttachPoint);
		WeaponMesh1P->SetRelativeLocation(WeaponMesh1PEquippedRelativeLocation);
		WeaponMesh1P->SetRelativeRotation(FRotator(0, 0, -90.0f));
		
		if(OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First)
		{
			WeaponMesh1P->SetVisibility(true, true);			
		}
		else
		{
			WeaponMesh1P->SetVisibility(false, true);			
		}
	}
	
	if (WeaponMesh3P)
	{		
		// FName(TEXT("Weapon_rSocket"))
		WeaponMesh3P->AttachToComponent(OwningCharacter->GetMesh(), AttachmentRules, AttachPoint3P);
		
		// WeaponMesh3P->SetRelativeLocation(WeaponMesh3PEquippedRelativeLocation);
		// WeaponMesh3P->SetRelativeRotation(FRotator(0, 0, -90.0f));
		WeaponMesh3P->CastShadow = true;
		// WeaponMesh3P->bCastHiddenShadow = true;

		if(OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First)
		{
			WeaponMesh3P->SetVisibility(true, true); // Without this, the weapon's 3p shadow doesn't show
			WeaponMesh3P->SetVisibility(false, true);
		}
		else
		{
			WeaponMesh3P->SetVisibility(true, true);
		}
	}
	
	
	// Attach the weapon to the First Person Character
	
	// AttachToComponent(OwningCharacter->GetMesh(), AttachmentRules, FName(TEXT("Weapon_rSocket")));
	// WeaponMesh3P->SetVisibility(true, true);
	// WeaponMesh3P->CastShadow = true;
	
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// switch bHasRifle so the animation blueprint can switch to another animation set
	// OwningCharacter->SetHasRifle(true);

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

			EnhancedInputComponent->BindAction(PrecisionAimAction, ETriggerEvent::Triggered, this, &ACHGun::StartPrecisionAim);
			EnhancedInputComponent->BindAction(CancelPrecisionAimAction, ETriggerEvent::Triggered, this, &ACHGun::StopPrecisionAim);

			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ACHGun::Reload);
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

	// WeaponMesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh1P->SetVisibility(false, true);
	WeaponMesh1P->CastShadow = false;
	
	// WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh3P->SetVisibility(false, false);
	WeaponMesh3P->CastShadow = false;
	
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
	AttachToComponent(OwningCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/*if (OwningCharacter->GetCurrentWeapon() != this)
	{
		WeaponMesh3P->CastShadow = false;
		WeaponMesh3P->SetVisibility(true, true);
		WeaponMesh3P->SetVisibility(false, true);
	}*/
}

void ACHGun::PickUpOnTouch(ACHCharacterBase* InCharacter)
{
	// bIsEquipped = true;
	// InCharacter->AddWeaponToInventory(this,true);
}

uint8 ACHGun::FireByAI()
{
	if(ShootingType == EWeaponShootType::LineTrace)
	{
		if(OwningCharacter)	OwningCharacter->SetAiming(true);
		
		FireLine();
		GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHGun::EndShoot, FireInterval, false);	// End Attack.
		
		return true;
	}
	if(ShootingType == EWeaponShootType::Projectile)
	{
		if(OwningCharacter)	OwningCharacter->SetAiming(true);
		
		FireProjectile();
		GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHGun::EndShoot, FireInterval, false);
		return true;
	}
	return false;
}

void ACHGun::EndShoot()
{
	if(!bIsEquipped) return;
	
	// if(OwningCharacter)	OwningCharacter->SetCombatMode(false);
	GetWorld()->GetTimerManager().ClearTimer(ShootTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);		// 이래야 EndShoot 반복 가능
	// 다끝나고 초기화
	bTrigger = false;
	
	OwningCharacter->NotifyComboActionEnd();	 // AttackTask return Succeeded
}

void ACHGun::FireLine()
{
	// 쏘는 몽타주가 여기 있다. 총알이 다 차거나 재장전 중일 때 예외처리는 여기서 해야할 듯. 
	if(!bIsEquipped) return;
	if(bReloading || CurrentAmmoInClip <= 0) return;
		
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

	AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
	if(AIController)
	{		
		// Location, Rotation을 총구로 설정하기
		Rotation = GetOwner()->GetActorRotation();
		Location = GetOwner()->GetActorLocation() + Rotation.RotateVector(MuzzleOffset);
	}
	
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
		// DrawDebugPoint(GetWorld(), Hit.Location, 5, FColor::Red, true);
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
	UAnimInstance* TPAnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
	UAnimInstance* FPAnimInstance = OwningCharacter->GetFirstPersonMesh()->GetAnimInstance();	
	if (TPAnimInstance)
	{
		TPAnimInstance->Montage_Play(Fire3PMontage, 1);		
	}
	if (FPAnimInstance)
	{
		if(OwningCharacter->GetScopeAiming()) FPAnimInstance->Montage_Play(ScopeFire1PMontage,1);
		else FPAnimInstance->Montage_Play(Fire1PMontage, 1);
	}
	if(!bInfiniteAmmo) CurrentAmmoInClip -= 1;	
}

void ACHGun::FireProjectile()
{
	if(!bIsEquipped) return;
	if(bReloading || CurrentAmmoInClip <= 0) return;
	
	if (OwningCharacter == nullptr || OwningCharacter->GetController() == nullptr)
	{
		return;
	}
	
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
	UAnimInstance* TPAnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
	UAnimInstance* FPAnimInstance = OwningCharacter->GetFirstPersonMesh()->GetAnimInstance();	
	if (TPAnimInstance)
	{
		TPAnimInstance->Montage_Play(Fire3PMontage, 1);		
	}
	if (FPAnimInstance)
	{
		if(OwningCharacter->GetScopeAiming()) FPAnimInstance->Montage_Play(ScopeFire1PMontage,1);
		else FPAnimInstance->Montage_Play(Fire1PMontage, 1);
	}
	
	if(!bInfiniteAmmo) CurrentAmmoInClip -= 1;
}

void ACHGun::PullTriggerLine()
{	
	if(!bIsEquipped) return;
	if(bReloading)
	{
		CancelPullTrigger();
		return;
	}

	OwningCharacter->bUseControllerRotationYaw = true;

	// not combat mode

	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdAim
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdPrecisionAim
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::FirstScopeAim)
	{
		OwningCharacter->SetAiming(true);

		if(FireMode == EFireMode::Automatic)
		{
			GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHGun::FireLine, FireInterval, true);
		}
		if(FireMode == EFireMode::SemiAutomatic)
		{
			FireLine();					
		}
	}

	// 아직 조준하지 않은 상태. 
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdCover)
	{
		// hold a gun
		OwningCharacter->SetAiming(true);
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

void ACHGun::PullTriggerProjectile()
{
	if(!bIsEquipped) return;
	if(bReloading)
	{
		CancelPullTrigger();
		return;
	}
	
	OwningCharacter->bUseControllerRotationYaw = true;

	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdAim
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdPrecisionAim
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::FirstScopeAim)
	{
		OwningCharacter->SetAiming(true);
		// GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHGun::FireProjectile, FireInterval, true);
		if(FireMode == EFireMode::Automatic)
		{
			GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHGun::FireProjectile, FireInterval, true);
		}
		if(FireMode == EFireMode::SemiAutomatic)
		{
			UE_LOG(LogTemp, Log, TEXT("ThirdAim|SemiAutomatic"));
			FireProjectile();
		}
	}
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdCover)
	{
		// hold a gun
		OwningCharacter->SetAiming(true);

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
				UE_LOG(LogTemp, Log, TEXT("Third|SemiAutomatic"));
			}, ShootingPreparationTime, false);	
		}
	}
	bTrigger = true;	
}

void ACHGun::CancelPullTrigger()
{
	if(!bIsEquipped) return;
	// if(bReloading) return;
	
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third)
		OwningCharacter->bUseControllerRotationYaw = false; 

	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdCover)
	{
		// Cancel holding a gun
		OwningCharacter->SetAiming(false);
	
		GetWorld()->GetTimerManager().ClearTimer(ShootTimerHandle);
	}
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	bTrigger = false;
	OwningCharacter->NotifyComboActionEnd();
}

void ACHGun::StartAim()
{
	if(!bIsEquipped) return;
	if(bReloading)
	{
		// cancel aim
		StopAim();
		return;
	}
	
	ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);
	if (PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::Third)
	{
		PlayerCharacter->SetCharacterControl(ECharacterControlType::ThirdAim);
	}

	if(PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdCover)
	{
		if(!PlayerCharacter->GetCovered())
		{
			UE_LOG(LogTemp,Warning,TEXT("Cover variable is not correct"));
		}		
		PlayerCharacter->SetCharacterControl(ECharacterControlType::ThirdAim);
		PlayerCharacter->SetCoveredAttackMotion(true);
	}

	if (PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::First)
	{
		// 조준 여부 변수 추가
		// PlayerCharacter->SetAiming(true);  // 근데 이건 밑에 코드에서 해줌
		if(PlayerCharacter->GetScopeAiming())
		{
			PlayerCharacter->SetCharacterControl(ECharacterControlType::FirstScopeAim);
		}
		else
		{
			PlayerCharacter->SetCharacterControl(ECharacterControlType::FirstAim);			
		}
	}
	// if Pull Triggering, pass
	if(!bTrigger) OwningCharacter->SetAiming(true);	
}

void ACHGun::StopAim()
{
	if(!bIsEquipped) return;
	// if(bReloading) return;
	
	ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);
	if (PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdAim
		|| PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdPrecisionAim)
	{
		if(PlayerCharacter->GetCovered())
		{
			PlayerCharacter->SetCharacterControl(ECharacterControlType::ThirdCover);
			PlayerCharacter->SetCoveredAttackMotion(false);			
		}
		else
		{
			PlayerCharacter->SetCharacterControl(ECharacterControlType::Third);			
		}
	}
		
	if (PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim
		|| PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::FirstScopeAim)
	{		
		PlayerCharacter->SetCharacterControl(ECharacterControlType::First);
		// 상태만 바꾸는 것 같지만 조준 변수는 밑에서 조정
	}
	
	if(!bTrigger)
	{
		OwningCharacter->SetAiming(false); // if PullTriggering, pass
	}
	else
	{
		OwningCharacter->bUseControllerRotationYaw = true;		
	}
	
}

void ACHGun::StartPrecisionAim()
{
	// 휠 올리면 호출되는 함수
	if(!bIsEquipped) return;
	
	ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);

	
	if(PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdAim)
	{
		// 조준경 bool 변수 -> 애니메이션에 전달
		PlayerCharacter->SetTPAimingCloser(true);
		// 카메라 위치 수정
		PlayerCharacter->SetCharacterControl(ECharacterControlType::ThirdPrecisionAim);
	}

	if(PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim)
	{
		PlayerCharacter->SetScopeAiming(true);
		PlayerCharacter->SetCharacterControl(ECharacterControlType::FirstScopeAim);
	}
	
	
}

void ACHGun::StopPrecisionAim()
{
	// 휠 내리면 호출되는 함수
	if(!bIsEquipped) return;
	
	ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);
	if(PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdPrecisionAim)
	{
		PlayerCharacter->SetTPAimingCloser(false);
		PlayerCharacter->SetCharacterControl(ECharacterControlType::ThirdAim);		
	}
	
	if(PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::FirstScopeAim)
	{
		PlayerCharacter->SetCharacterControl(ECharacterControlType::FirstAim);
		PlayerCharacter->SetScopeAiming(false);
	}	

}

void ACHGun::StopParticleSystem()
{
	// UE_LOG(LogTemp, Warning, TEXT("StopParticleSystem"));

	// Deactivate the ParticleSystemComponent to stop playing the particle effect
	Effect->Deactivate();
}

void ACHGun::Reload()
{
	if(bReloading) return;
	if(bInfiniteAmmo) return;
	// 소유한 총알 = 0
	if(CurrentAmmo == 0)
	{
		UE_LOG(LogTemp,Log,TEXT("Find Ammo"));
		return;
	}

	// 탄창에 총알 가득
	if(CurrentAmmoInClip == ClipSize)
	{
		UE_LOG(LogTemp,Log,TEXT("Full Ammo"));
		return;
	}

	// Reload
	bReloading = true;

	// 조준 중이라면 해제
	/*if(OwningCharacter->bIsFirstPersonPerspective)
	{
		OwningCharacter->SetCharacterControl(ECharacterControlType::First);
	}
	else
	{
		// 엄폐는 유지
		if(OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdCover)
		{
			OwningCharacter->SetCharacterControl(ECharacterControlType::ThirdCover);			
		}
		else
		{
			OwningCharacter->SetCharacterControl(ECharacterControlType::Third);			
		}
	}*/
	
	// if(OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdAim
	
	// 총 재장전 Animation Montage
	if(ReloadWeaponMontage)
	{
		WeaponMesh1P->GetAnimInstance()->Montage_Play(ReloadWeaponMontage);
		WeaponMesh3P->GetAnimInstance()->Montage_Play(ReloadWeaponMontage);				
	}
	
	// 플레이어 재장전 Animation Montage
	UAnimInstance* TPAnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
	UAnimInstance* FPAnimInstance = OwningCharacter->GetFirstPersonMesh()->GetAnimInstance();
	if(FPAnimInstance) FPAnimInstance->Montage_Play(Reload1PMontage,1);
	if(TPAnimInstance) TPAnimInstance->Montage_Play(Reload3PMontage, 1);

	// 탄창 증가

	// 총알이 있다면
	if(CurrentAmmo > 0)
	{
		int32 NeededAmmo;
		NeededAmmo = ClipSize - CurrentAmmoInClip;
		if(CurrentAmmo > NeededAmmo)
		{
			CurrentAmmoInClip = ClipSize;
			CurrentAmmo -= NeededAmmo;
		}
		else
		{
			CurrentAmmoInClip += CurrentAmmo;
			CurrentAmmo = 0;
		}		
	}
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, [this]()
	{
		bReloading = false;		
	}, ReloadInterval, false);
	
}
