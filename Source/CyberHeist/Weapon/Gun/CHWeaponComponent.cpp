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

	//MuzzleFlashComponent->Activate(true);

	//float Duration = 0.1f; // Set the duration time in seconds
	//FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	//TimerManager.SetTimer(DurationTimerHandle, this, &UCHWeaponComponent::StopParticleSystem, Duration, false);

	

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
			World->SpawnActor<ACHProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
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
	if (Character->CurrentCharacterControlType == ECharacterControlType::ThirdAim
		|| Character->CurrentCharacterControlType == ECharacterControlType::FirstAim)
	{
		Fire();
	}
	if (Character->CurrentCharacterControlType == ECharacterControlType::Third ||
		Character->CurrentCharacterControlType == ECharacterControlType::First)
	{
		// hold a gun
		Character->SetCombatMode(true);

		// holding a gun delay
		Character->GetWorldTimerManager().SetTimer(ShootTimerHandle, [this]()
			{
				Fire();

			}, ShootingPreparationTime, false);
	}
}

void UCHWeaponComponent::CancelPullTrigger()
{
	Cast<ACHCharacterPlayer>(Character);
	if (Character->CurrentCharacterControlType == ECharacterControlType::Third ||
		Character->CurrentCharacterControlType == ECharacterControlType::First)
	{
		// Cancel holding a gun
		Character->SetCombatMode(false);
		
		Character->GetWorldTimerManager().ClearTimer(ShootTimerHandle);
	}
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
