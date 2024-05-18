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
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Player/CHPlayerController.h"

#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"

ACHGunRPG::ACHGunRPG()
{
	DefaultFireMode = ECHFireMode::SemiAuto;

	ScopeMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(FName("ScopeMesh1P"));
	ScopeMesh1P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ScopeMesh1P->CastShadow = false;
	ScopeMesh1P->SetVisibility(false, true);
	ScopeMesh1P->SetupAttachment(WeaponMesh1P);
	// ScopeMesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;

	ScopeMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ScopeMesh3P"));
	ScopeMesh3P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ScopeMesh3P->SetupAttachment(WeaponMesh3P);
	ScopeMesh3P->CastShadow = true;
	ScopeMesh3P->SetVisibility(true, true);
	// ScopeMesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;

	MissileMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(FName("MissileMesh1P"));
	MissileMesh1P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MissileMesh1P->CastShadow = false;
	MissileMesh1P->SetVisibility(false, true);
	MissileMesh1P->SetupAttachment(WeaponMesh1P);
	// MissileMesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;

	MissileMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MissileMesh3P"));
	MissileMesh3P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MissileMesh3P->SetupAttachment(WeaponMesh3P);
	MissileMesh3P->CastShadow = true;
	MissileMesh3P->SetVisibility(true, true);
	// MissileMesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	
	Lens = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lens"));
	Lens->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Lens->SetupAttachment(CollisionComp);
	Lens->CastShadow = true;
	Lens->SetVisibility(false);	
}

void ACHGunRPG::FireActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	StopAim();
	Reload();
}

void ACHGunRPG::Equip()
{
	Super::Equip();
	
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
			WeaponMesh1P->SetVisibility(true, true);
			ScopeMesh1P->SetVisibility(true, true);
			MissileMesh1P->SetVisibility(true, true);		
		}
		else
		{
			WeaponMesh1P->SetVisibility(false, true);			
			ScopeMesh1P->SetVisibility(false, true);
			MissileMesh1P->SetVisibility(false, true);		
		}
		
		Lens->SetVisibility(false);
	}
	
	if (WeaponMesh3P)
	{		
		WeaponMesh3P->AttachToComponent(OwningCharacter->GetMesh(), AttachmentRules, AttachPoint3P);
		WeaponMesh3P->CastShadow = true;
		WeaponMesh3P->bCastHiddenShadow = true;
		ScopeMesh3P->CastShadow = true;
		ScopeMesh3P->bCastHiddenShadow = true;
		MissileMesh3P->CastShadow = true;
		MissileMesh3P->bCastHiddenShadow = true;
		
		if(OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First)
		{
			// WeaponMesh3P->SetVisibility(true, true); // Without this, the weapon's 3p shadow doesn't show
			WeaponMesh3P->SetVisibility(false, true);
			ScopeMesh3P->SetVisibility(false, true);
			MissileMesh3P->SetVisibility(false, true);
		}
		else
		{
			WeaponMesh3P->SetVisibility(true, true);
			ScopeMesh3P->SetVisibility(true, true);
			MissileMesh3P->SetVisibility(true, true);
		}
	}
	
	// Set up action bindings
	if (APlayerController* PlayerController = CastChecked<APlayerController>(OwningCharacter->GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
				
		if(Subsystem->HasMappingContext(FireMappingContext))
		{
			UE_LOG(LogTemp, Log, TEXT("[CHRPG] Have FireMappingContext"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[CHRPG] No FireMappingContext"));
			Subsystem->AddMappingContext(FireMappingContext, 0);

			// only once
			// 캐릭터에게 변수 줘야 함. 총이 캐릭터 변수 가지고 판단하기.
			if(!OwningCharacter->GetbHasRPGInputBindings())
			{
				SetupWeaponInputComponent();
				OwningCharacter->SetbHasRPGInputBindings(true);
			}			
		}
	}
}

void ACHGunRPG::UnEquip()
{
	Super::UnEquip();

	ScopeMesh1P->SetVisibility(false, true);
	ScopeMesh1P->CastShadow = false;
	MissileMesh1P->SetVisibility(false, true);
	MissileMesh1P->CastShadow = false;
	
	ScopeMesh3P->SetVisibility(false, true);
	ScopeMesh3P->CastShadow = false;
	MissileMesh3P->SetVisibility(false, true);
	MissileMesh3P->CastShadow = false;

	if (APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
	{		
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if(Subsystem->HasMappingContext(FireMappingContext))
			{
				Subsystem->RemoveMappingContext(FireMappingContext);
				UE_LOG(LogTemp, Log, TEXT("[ACHGunRPG] Removed %s"), *FireMappingContext->GetName());
			}
			
		}
	}
}

void ACHGunRPG::Fire()
{
	Super::Fire();
	if(!bIsEquipped) return;
	if(bReloading || CurrentAmmoInClip <= 0) return;
	UE_LOG(LogTemp, Log, TEXT("Fire() 1"));
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
	// float Duration = 0.1f;				// Set the duration time in seconds
	// GetWorldTimerManager().SetTimer(DurationTimerHandle, this, &ACHGunRPG::StopParticleSystem, Duration, false);

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
	if (World)
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
		// CurrentAmmoInClip--;
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
	if(!bInfiniteAmmo) CurrentAmmoInClip -= 1;
}

void ACHGunRPG::EndShoot()
{
	Super::EndShoot();
}

void ACHGunRPG::PullTrigger()
{
	Super::PullTrigger();
	if(!bIsEquipped) return;
	UE_LOG(LogTemp, Log, TEXT("PullTrigger() 1"));
	if(bReloading)
	{
		CancelPullTrigger();
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("PullTrigger() 2"));
	if(CurrentAmmoInClip <= 0)
	{
		if(CurrentAmmo <= 0)
		{
			UE_LOG(LogTemp,Log,TEXT("Find Ammo"));
			return;
		}
		Reload();
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("PullTrigger() 3"));
	// 미사일 메시 비활성화.
	MissileMesh1P->SetVisibility(false);
	MissileMesh3P->SetVisibility(false);
	
	if(OwningCharacter->GetNearWall()) return;
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
	
	// 엄폐한 상태에서 바로 쏘지는 못함. 조준을 꼭 해야 함. 
	if(OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdCover) return;

	// 아직 조준하지 않은 상태. 
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First)
	{
		if(OwningCharacter->GetNearWall())
		{
			CancelPullTrigger();
			return;
		}
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
	bHoldGun = false;
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
			if(APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
			{
				PlayerController->SetViewTargetWithBlend(this,0.1);
				OwningCharacter->GetFirstPersonMesh()->SetVisibility(false);	// 팔 보이게 하기
				Lens->SetVisibility(true);
			}
		}
		else
		{
			PlayerCharacter->SetCharacterControl(ECharacterControlType::FirstAim);
			Lens->SetVisibility(false);
		}
	}

	// if Pull Triggering, pass
	if(OwningCharacter->GetNearWall()) return;
	if(!bTrigger) OwningCharacter->SetAiming(true);	
}

void ACHGunRPG::StopAim()
{
	Super::StopAim();

	if(!bIsEquipped) return;
	// if(bReloading) return;
	bHoldGun = true;
	ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);

	PlayerCharacter->SetMappingContextPriority(FireMappingContext, 0);

	if(PlayerCharacter)
	{
		if (PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdAim
			|| PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdPrecisionAim)
		{
			if(PlayerCharacter->GetCovered())
			{
				PlayerCharacter->SetCharacterControl(ECharacterControlType::ThirdCover);
				PlayerCharacter->ReturnCover();
				PlayerCharacter->SetCoveredAttackMotion(false);			
			}
			else
			{
				PlayerCharacter->SetCharacterControl(ECharacterControlType::Third);		
			}
		}

		if (PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim)
		{
			PlayerCharacter->SetCharacterControl(ECharacterControlType::First);
			Lens->SetVisibility(false);
		}
	
		if(PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::FirstScopeAim)
		{
			PlayerCharacter->SetCharacterControl(ECharacterControlType::First);
			Lens->SetVisibility(false);
		
			if(APlayerController* PlayerController = CastChecked<APlayerController>(OwningCharacter->GetController()))
			{			
				PlayerController->SetViewTargetWithBlend(OwningCharacter,0.1f);
				OwningCharacter->GetFirstPersonMesh()->SetVisibility(true);
			}
		}
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
	if(bReloading)
	{
		// cancel aim
		StopAim();
		return;
	}
	bHoldGun = false;
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
		if(APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
		{
			Lens->SetVisibility(true);
			PlayerController->SetViewTargetWithBlend(this,0.2);
			OwningCharacter->GetFirstPersonMesh()->SetVisibility(false);
		}
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
		Lens->SetVisibility(false);
		
		OwningCharacter->GetFirstPersonMesh()->SetVisibility(true);
		if(ACHPlayerController* PlayerController = CastChecked<ACHPlayerController>(OwningCharacter->GetController()))
		{
			PlayerController->SetViewTargetWithBlend(OwningCharacter,0.2);	
		}
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

void ACHGunRPG::SetupWeaponInputComponent()
{
	Super::SetupWeaponInputComponent();
	
	if (APlayerController* PlayerController = CastChecked<APlayerController>(OwningCharacter->GetController()))
	{
		// 무기를 가진 적이 있는지 확인하고 가지고 있으면 bind는 하지 않는다. 
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{		
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ACHGunRPG::PullTrigger);	
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Canceled, this, &ACHGunRPG::CancelPullTrigger);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ACHGunRPG::StartAim);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ACHGunRPG::StopAim);

			// EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ACHGunRPG::StartAim);
			// EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Canceled, this, &ACHGunRPG::StopAim);
			
			EnhancedInputComponent->BindAction(PrecisionAimAction, ETriggerEvent::Triggered, this, &ACHGunRPG::StartPrecisionAim);
			EnhancedInputComponent->BindAction(CancelPrecisionAimAction, ETriggerEvent::Triggered, this, &ACHGunRPG::StopPrecisionAim);
			// EnhancedInputComponent->BindAction(FirstLookAction, ETriggerEvent::Triggered, this, &ACHGunRPG::FirstLook);
		}
	}
}

void ACHGunRPG::FirstLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	if(OwningCharacter)
	{		
		ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);
		if(PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::FirstScopeAim)
		{
			// WeaponMesh1P->AddRelativeRotation(FRotator(0,0,LookAxisVector.Y));
			FRotator NewRotation = WeaponMesh1P->GetRelativeRotation() + FRotator(0,0,LookAxisVector.Y);

			UE_LOG(LogTemp, Log, TEXT("NewRotation P : %f, Y : %f R : %f"), NewRotation.Pitch,NewRotation.Yaw, NewRotation.Roll)
			
			/*if(NewRotation.Roll > -10.0f)
			{
				NewRotation.Roll = -90.0f;
			}
			if(NewRotation.Roll < -170.0f)
			{
				NewRotation.Roll = -170.0f;
			}*/
			/*if(NewRotation.Roll < -90.0f && NewRotation.Roll > -170.0f)
			{
				
			}*/
			NewRotation.Roll = FMath::Clamp(NewRotation.Roll, -170.0f, -10.0f);
			

			// 잘되는 버전			
			WeaponMesh1P->SetRelativeRotation(NewRotation);
		}
		else
		{
			WeaponMesh1P->SetRelativeRotation(FRotator(0, 0, -90.0f));
		}
	}
}

void ACHGunRPG::SetWeaponMeshVisibility(uint8 bVisible)
{
	Super::SetWeaponMeshVisibility(bVisible);

	Lens->SetVisibility(bVisible == 1);
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
