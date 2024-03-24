// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/CHWeaponComponent.h"
#include "Character/CHCharacterBase.h"
#include "Character/CHCharacterPlayer.h"
#include "CHProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Particles/ParticleSystemComponent.h"

UCHWeaponComponent::UCHWeaponComponent()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> WeaponMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/AssetPacks/ShooterGame/Weapons/Rifle.Rifle'"));
	if (WeaponMeshRef.Object)
	{
		SetSkeletalMesh(WeaponMeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> FireSoundRef(TEXT("/Script/Engine.SoundWave'/Game/AssetPacks/ShooterGame/Sounds/Weapon_AssaultRifle/Mono/AssaultRifle_Shot02.AssaultRifle_Shot02'"));
	if (FireSoundRef.Object)
	{
		FireSound = FireSoundRef.Object;
	}	

	static ConstructorHelpers::FObjectFinder<UAnimMontage> FireMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/AssetPacks/ParagonWraith/Characters/Heroes/Wraith/Animations/Fire_A_Fast_V1_Montage.Fire_A_Fast_V1_Montage'"));
	if (FireMontageRef.Object)
	{
		FireAnimation = FireMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ImpactRef(TEXT("/Script/Engine.ParticleSystem'/Game/AssetPacks/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_MuzzleFlash.P_Wraith_Primary_MuzzleFlash'"));
	if (ImpactRef.Object)
	{
		MuzzleFlash = ImpactRef.Object;
	}

	/*static ConstructorHelpers::FObjectFinder<UInputMappingContext> FireIMCRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/CyberHeist/Input/IMC_Weapon.IMC_Weapon'"));
	if (FireIMCRef.Succeeded())
	{
		FireMappingContext = FireIMCRef.Object;
	}*/

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShootRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_Shoot.IA_Shoot'"));
	if (nullptr != InputActionShootRef.Object)
	{
		FireAction = InputActionShootRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionAimRef(TEXT("/Script/EnhancedInput.InputAction'/Game/CyberHeist/Input/Actions/IA_Aim.IA_Aim'"));
	if (nullptr != InputActionAimRef.Object)
	{
		AimAction = InputActionAimRef.Object;
	}

	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UCHWeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}
	
	if (MuzzleFlash != nullptr)
	{
		AActor* Owner = GetOwner();
		if (Owner != nullptr)
		{
			UParticleSystemComponent* ParticleComponent = UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Owner->GetRootComponent(), TEXT("MuzzleFlashSocket"));
			if (ParticleComponent != nullptr)
			{
				// 활성화 후 일정 시간 후에 비활성화
				ParticleComponent->Activate(true);
				float Duration = 0.1f; // 비활성화할 시간(초)
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
	}	

	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// Spawn the projectile at the muzzle
			ACHProjectile* Projectile = World->SpawnActor<ACHProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			Projectile->SetOwner(Character);
		}
	}

	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}

	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 2.f);
		}
	}
}

void UCHWeaponComponent::AttachWeapon(ACHCharacterPlayer* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr)
	{
		return;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh(), AttachmentRules, FName(TEXT("Weapon_rSocket")));

	// switch bHasRifle so the animation blueprint can switch to another animation set
	Character->SetHasRifle(true);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			// EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UCHWeaponComponent::Fire);

			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &UCHWeaponComponent::PullTrigger);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Canceled, this, &UCHWeaponComponent::CancelPullTrigger);

			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &UCHWeaponComponent::StartAim);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Canceled, this, &UCHWeaponComponent::StopAim);

			// UI도 달아주기
			// UCHHUDWidget를 가져와도 문제인게 SetupHUDWidget은 HUD의 Native에서 초기화한다. 
			// 그래서 이벤트 등록을 SetupHUDWidget 밖에 할 수 없다는 제약이 생긴다. 과연 이게 맞는 걸까? 
			// OnCombat.AddUObject(InHUDWidget, &UCHHUDWidget::SetCombatMode);
		}
	}
}

void UCHWeaponComponent::PullTrigger()
{	
	Character->bUseControllerRotationYaw = true;
	if (Character->CurrentCharacterControlType == ECharacterControlType::ThirdAim
		|| Character->CurrentCharacterControlType == ECharacterControlType::FirstAim)
	{
		// Fire();
		Character->SetCombatMode(true);
		GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &UCHWeaponComponent::Fire, FireInterval, true);
	}
	if (Character->CurrentCharacterControlType == ECharacterControlType::Third ||
		Character->CurrentCharacterControlType == ECharacterControlType::First)
	{
		// hold a gun
		Character->SetCombatMode(true);

		// holding a gun delay
		GetWorld()->GetTimerManager().SetTimer(ShootTimerHandle, [this]()
		{
			// Fire();
			// Activate the timer to continuously fire at intervals
			GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &UCHWeaponComponent::Fire, FireInterval, true);
		}, ShootingPreparationTime, false);		
	}
}

void UCHWeaponComponent::CancelPullTrigger()
{
	// 뗏다고 false되면 안된다. 조준 중에 사격하다가 사격만 안하면 false가 되어 카메라 이상..
	// 조준 중이면 계속 bUseControllerRotationYaw는 유지
	// 따라서 조건문을 주가해야 한다. 
	// if(!현재 aim) 변경
	if (Character->CurrentCharacterControlType != ECharacterControlType::ThirdAim &&
		Character->CurrentCharacterControlType != ECharacterControlType::FirstAim)
		Character->bUseControllerRotationYaw = false;

	// Character->bUseControllerRotationYaw = false; 

	if (Character->CurrentCharacterControlType == ECharacterControlType::Third ||
		Character->CurrentCharacterControlType == ECharacterControlType::First)
	{
		// Cancel holding a gun
		Character->SetCombatMode(false);
		
		GetWorld()->GetTimerManager().ClearTimer(ShootTimerHandle);
	}
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void UCHWeaponComponent::StartAim()
{
	if (Character->CurrentCharacterControlType == ECharacterControlType::Third)
	{
		Character->SetCharacterControl(ECharacterControlType::ThirdAim);
		Character->SetCombatMode(true);
	}
	if (Character->CurrentCharacterControlType == ECharacterControlType::First)
	{
		Character->SetCharacterControl(ECharacterControlType::FirstAim);
		Character->SetCombatMode(true);		
	}
}

void UCHWeaponComponent::StopAim()
{	
	if (Character->CurrentCharacterControlType == ECharacterControlType::ThirdAim)
	{
		Character->SetCharacterControl(ECharacterControlType::Third);
		Character->SetCombatMode(false);
	}
	if (Character->CurrentCharacterControlType == ECharacterControlType::FirstAim)
	{
		Character->SetCharacterControl(ECharacterControlType::First);
		Character->SetCombatMode(false);
	}
	// 총 쏘는 중에 aim만 풀려도 계속 전방 주시 모드
	// if(총 쏘는 중) Character->bUseControllerRotationYaw = true;
	// 근데 총 쏘는 중은 어캐 알지??? -> Cancel에서 다 해결
	// if (Character->bCombatMode) Character->bUseControllerRotationYaw = true;
}

void UCHWeaponComponent::StopParticleSystem()
{
	// UE_LOG(LogTemp, Warning, TEXT("StopParticleSystem"));
	// Deactivate the ParticleSystemComponent to stop playing the particle effect
	// MuzzleFlashComponent->Deactivate();
}

void UCHWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
}
