// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/CHGunRifle.h"
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
#include "Engine/SkeletalMeshSocket.h"

ACHGunRifle::ACHGunRifle() 
{
}

void ACHGunRifle::Equip()
{
	Super::Equip();

	if(!bIsEquipped) return;
	OwningCharacter->SetHasRifle(true);

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

	const USkeletalMeshSocket* HandSocket = OwningCharacter->GetMesh()->GetSocketByName(AttachPoint3P);
	if(HandSocket)
	{
		HandSocket->AttachActor(this,OwningCharacter->GetMesh());
	}
	
	if (WeaponMesh1P)
	{
		WeaponMesh1P->AttachToComponent(OwningCharacter->GetFirstPersonMesh(), AttachmentRules, AttachPoint1P);		
		WeaponMesh1P->SetRelativeRotation(FRotator(0, 0, -90.0f));
	
		if(OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First)
		{
			WeaponMesh1P->SetVisibility(true);
		}
		else
		{
			WeaponMesh1P->SetVisibility(false);
		}
	}
	
	if (WeaponMesh3P)
	{		
		WeaponMesh3P->AttachToComponent(OwningCharacter->GetMesh(), AttachmentRules, AttachPoint3P);
		WeaponMesh3P->CastShadow = true;
		// WeaponMesh3P->bCastHiddenShadow = true;

		if(OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First)
		{
			// WeaponMesh3P->SetVisibility(true, true); // Without this, the weapon's 3p shadow doesn't show
			WeaponMesh3P->SetVisibility(false);
		}
		else
		{
			WeaponMesh3P->SetVisibility(true);
		}
	}
	
	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
	{		
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if(Subsystem->HasMappingContext(FireMappingContext))
		{
			return;
		}
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			
			Subsystem->AddMappingContext(FireMappingContext, 0);
		

			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
			{
				EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ACHGunRifle::PullTrigger);	
				EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Canceled, this, &ACHGunRifle::CancelPullTrigger);
				EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ACHGunRifle::StartAim);
				EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Canceled, this, &ACHGunRifle::StopAim);
				EnhancedInputComponent->BindAction(PrecisionAimAction, ETriggerEvent::Triggered, this, &ACHGunRifle::StartPrecisionAim);
				EnhancedInputComponent->BindAction(CancelPrecisionAimAction, ETriggerEvent::Triggered, this, &ACHGunRifle::StopPrecisionAim);
				EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ACHGunRifle::Reload);
			}
		
	}
}

void ACHGunRifle::UnEquip()
{
	Super::UnEquip();
}

void ACHGunRifle::Fire()
{
	Super::Fire();
	// 쏘는 몽타주가 여기 있다. 총알이 다 차거나 재장전 중일 때 예외처리는 여기서 해야할 듯. 
	if(!bIsEquipped) return;
	if(bReloading || CurrentAmmoInClip <= 0) return;
		
	// UE_LOG(LogTemp, Warning, TEXT("PullTrigger"));
	// UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh, TEXT("MuzzleFlashSocket"));
	Effect->Activate(true);
	float Duration = 0.1f; // Set the duration time in seconds
	GetWorldTimerManager().SetTimer(DurationTimerHandle, this, &ACHGunBase::StopParticleSystem, Duration, false);

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

void ACHGunRifle::PullTrigger()
{
	Super::PullTrigger();
	if(!bIsEquipped) return;
	if(bReloading)
	{
		CancelPullTrigger();
		return;
	}

	OwningCharacter->bUseControllerRotationYaw = true;

	// not aiming mode
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdAim
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdPrecisionAim
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::FirstScopeAim)
	{
		OwningCharacter->SetAiming(true);

		if(FireMode == ECHFireMode::FullAuto)
		{
			GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHGunRifle::Fire, FireInterval, true);
		}
		if(FireMode == ECHFireMode::SemiAuto)
		{
			Fire();					
		}
	}

	// 아직 조준하지 않은 상태. 
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdCover)
	{
		// hold a gun
		OwningCharacter->SetAiming(true);

		if(FireMode == ECHFireMode::FullAuto)
		{
			// holding a gun delay
			GetWorld()->GetTimerManager().SetTimer(ShootTimerHandle, [this]()
			{
				GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHGunRifle::Fire, FireInterval, true);
			}, ShootingPreparationTime, false);	
		}
		if(FireMode == ECHFireMode::SemiAuto)
		{
			GetWorld()->GetTimerManager().SetTimer(ShootTimerHandle, [this]()
			{
				Fire();
			}, ShootingPreparationTime, false);	
		}
	}
	bTrigger = true;
}

void ACHGunRifle::CancelPullTrigger()
{
	Super::CancelPullTrigger();
	if(!bIsEquipped) return;
	
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

void ACHGunRifle::StartAim()
{
	Super::StartAim();
	if(!bIsEquipped) return;
	if(bReloading)
	{
		// cancel aim
		StopAim();
		return;
	}
	
	ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);

	PlayerCharacter->SetMappingContextPriority(FireMappingContext, 2);
	
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

void ACHGunRifle::StopAim()
{
	Super::StopAim();

	if(!bIsEquipped) return;
	// if(bReloading) return;
	
	ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);

	PlayerCharacter->SetMappingContextPriority(FireMappingContext, 0);
	
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

void ACHGunRifle::StartPrecisionAim()
{
	Super::StartPrecisionAim();
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

void ACHGunRifle::StopPrecisionAim()
{
	Super::StopPrecisionAim();
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

void ACHGunRifle::Reload()
{
	Super::Reload();
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
	if(OwningCharacter->IsInFirstPersonPerspective())
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
	}
	
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

void ACHGunRifle::SetOwningCharacter(ACHCharacterBase* InOwningCharacter)
{
	Super::SetOwningCharacter(InOwningCharacter);
}

void ACHGunRifle::PickUpOnTouch(ACHCharacterBase* InCharacter)
{
	Super::PickUpOnTouch(InCharacter);
}

void ACHGunRifle::StopParticleSystem()
{
	Super::StopParticleSystem();
}
