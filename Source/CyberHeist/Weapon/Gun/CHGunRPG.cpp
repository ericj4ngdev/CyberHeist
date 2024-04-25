// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/CHGunRPG.h"
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

ACHGunRPG::ACHGunRPG()
{
	DefaultFireMode = ECHFireMode::SemiAuto;

	ScopeMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(FName("ScopeMesh1P"));
	ScopeMesh1P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ScopeMesh1P->CastShadow = false;
	ScopeMesh1P->SetVisibility(false, true);
	ScopeMesh1P->SetupAttachment(WeaponMesh1P);
	ScopeMesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;

	ScopeMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ScopeMesh3P"));
	ScopeMesh3P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ScopeMesh3P->SetupAttachment(WeaponMesh3P);
	ScopeMesh3P->CastShadow = true;
	ScopeMesh3P->SetVisibility(true, true);
	ScopeMesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;

	MissileMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(FName("MissileMesh1P"));
	MissileMesh1P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MissileMesh1P->CastShadow = false;
	MissileMesh1P->SetVisibility(false, true);
	MissileMesh1P->SetupAttachment(WeaponMesh1P);
	MissileMesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;

	MissileMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MissileMesh3P"));
	MissileMesh3P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MissileMesh3P->SetupAttachment(WeaponMesh3P);
	MissileMesh3P->CastShadow = true;
	MissileMesh3P->SetVisibility(true, true);
	MissileMesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	
	
}

void ACHGunRPG::FireActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	Reload();
}

void ACHGunRPG::Equip()
{
	Super::Equip();

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
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ACHGunRPG::PullTrigger);	
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Canceled, this, &ACHGunRPG::CancelPullTrigger);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ACHGunRPG::StartAim);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Canceled, this, &ACHGunRPG::StopAim);
			EnhancedInputComponent->BindAction(PrecisionAimAction, ETriggerEvent::Triggered, this, &ACHGunRPG::StartPrecisionAim);
			EnhancedInputComponent->BindAction(CancelPrecisionAimAction, ETriggerEvent::Triggered, this, &ACHGunRPG::StopPrecisionAim);
			// EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ACHGunRPG::Reload);
		}
	}
}

void ACHGunRPG::UnEquip()
{
	Super::UnEquip();
}

void ACHGunRPG::Fire()
{
	Super::Fire();
	if(!bIsEquipped) return;
	if(bReloading || CurrentAmmoInClip <= 0) return;
	
	if (OwningCharacter == nullptr || OwningCharacter->GetController() == nullptr)
	{
		return;
	}

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

	FVector Start;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Start, Rotation);
	
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	
	AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
	if(AIController)
	{		
		// Location, Rotation을 총구로 설정하기
		Rotation = GetOwner()->GetActorRotation();
		Start = GetOwner()->GetActorLocation() + Rotation.RotateVector(MuzzleOffset);
	}
	
	// DrawDebugCamera(GetWorld(), Location, Rotation, 90, 2, FColor::Red, true);
	
	FVector End = Start + Rotation.Vector() * MaxRange;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult,Start,End, ECollisionChannel::ECC_GameTraceChannel1, Params);
	FVector HitTarget = FVector{};

	// ====================================
	
	// Try and play effect
	float Duration = 0.1f;				// Set the duration time in seconds
	GetWorldTimerManager().SetTimer(DurationTimerHandle, this, &ACHGunRPG::StopParticleSystem, Duration, false);

	//Set Spawn Collision Handling Override
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.Owner = GetOwner();
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	ActorSpawnParams.Instigator = InstigatorPawn;
	ACHProjectile* Projectile = nullptr;

	
	// Try and fire a projectile
	if (ProjectileClass  == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("ProjectileClass is null"));
		return;
	}
	
	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		if(OwningCharacter->IsInFirstPersonPerspective())
		{
			const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh1P->GetSocketByName(FName("MuzzleOffset"));
			const FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh1P);
			
			HitTarget = HitResult.ImpactPoint;
			FVector ToTarget = HitTarget - SocketTransform.GetLocation();
			FRotator TargetRotation = ToTarget.Rotation();
			
			SpawnLocation = SocketTransform.GetLocation(); 
			SpawnRotation = (bHit ? TargetRotation : SocketTransform.GetRotation().Rotator());
			
			Projectile = World->SpawnActor<ACHProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			if(Projectile) Projectile->SetOwner(OwningCharacter);
		}
		else
		{
			const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh3P->GetSocketByName(FName("MuzzleOffset"));
			const FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh3P);

			HitTarget = HitResult.ImpactPoint;
			FVector ToTarget = HitTarget - SocketTransform.GetLocation();
			FRotator TargetRotation = ToTarget.Rotation();
						
			SpawnLocation = SocketTransform.GetLocation(); 
			SpawnRotation = (bHit ? TargetRotation : SocketTransform.GetRotation().Rotator());

			Projectile = World->SpawnActor<ACHProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			if(Projectile) Projectile->SetOwner(OwningCharacter);
		}
		// ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		CurrentAmmoInClip--;
	}
	
	

	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, OwningCharacter->GetActorLocation());
	}

	// Get the animation object for the arms mesh
	UAnimInstance* TPAnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
	UAnimInstance* FPAnimInstance = OwningCharacter->GetFirstPersonMesh()->GetAnimInstance();	

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ACHGunRPG::FireActionEnd);
	
	if (TPAnimInstance)
	{
		TPAnimInstance->Montage_Play(Fire3PMontage, 1);
		FPAnimInstance->Montage_SetEndDelegate(EndDelegate, Fire3PMontage);
	}
	if (FPAnimInstance)
	{
		if(OwningCharacter->GetScopeAiming())
		{
			FPAnimInstance->Montage_Play(ScopeFire1PMontage,1);
			FPAnimInstance->Montage_SetEndDelegate(EndDelegate, ScopeFire1PMontage);
		}
		else
		{
			FPAnimInstance->Montage_Play(Fire1PMontage, 1);
			FPAnimInstance->Montage_SetEndDelegate(EndDelegate, Fire1PMontage);
		}		
	}

	/*FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ACHGunRPG::FireActionEnd);*/

	
	if(!bInfiniteAmmo) CurrentAmmoInClip -= 1;
}

void ACHGunRPG::PullTrigger()
{
	Super::PullTrigger();
	if(!bIsEquipped) return;
	if(bReloading)
	{
		CancelPullTrigger();
		return;
	}

	// 미사일 메시 비활성화.
	MissileMesh1P->SetVisibility(false);
	MissileMesh3P->SetVisibility(false);
	
	OwningCharacter->bUseControllerRotationYaw = true;

	// not aiming mode
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdAim
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdPrecisionAim
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::FirstScopeAim)
	{
		OwningCharacter->SetAiming(true);
		Fire();					
		
	}

	// 아직 조준하지 않은 상태. 
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdCover)
	{
		// hold a gun
		OwningCharacter->SetAiming(true);

		GetWorld()->GetTimerManager().SetTimer(ShootTimerHandle, [this]()
		{
			Fire();
		}, ShootingPreparationTime, false);	
		
	}
	bTrigger = true;
}

void ACHGunRPG::CancelPullTrigger()
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

void ACHGunRPG::StartAim()
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

void ACHGunRPG::StopAim()
{
	Super::StopAim();

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

void ACHGunRPG::StartPrecisionAim()
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

void ACHGunRPG::StopPrecisionAim()
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

void ACHGunRPG::Reload()
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

void ACHGunRPG::SetOwningCharacter(ACHCharacterBase* InOwningCharacter)
{
	Super::SetOwningCharacter(InOwningCharacter);
}

void ACHGunRPG::PickUpOnTouch(ACHCharacterBase* InCharacter)
{
	Super::PickUpOnTouch(InCharacter);
}

void ACHGunRPG::StopParticleSystem()
{
	Super::StopParticleSystem();
}
