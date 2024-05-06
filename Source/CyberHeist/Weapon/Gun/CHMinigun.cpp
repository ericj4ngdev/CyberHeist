// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/CHMinigun.h"
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
#include "Sound/SoundCue.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/SkeletalMeshSocket.h"

ACHMinigun::ACHMinigun()
{
	DefaultFireMode = ECHFireMode::FullAuto;
	
	CannonMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(FName("CannonMesh1P"));
	CannonMesh1P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CannonMesh1P->CastShadow = false;
	CannonMesh1P->SetVisibility(false, true);
	CannonMesh1P->SetupAttachment(WeaponMesh1P);
	CannonMesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;

	CannonMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CannonMesh3P"));
	CannonMesh3P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CannonMesh3P->SetupAttachment(WeaponMesh3P);
	CannonMesh3P->CastShadow = true;
	CannonMesh3P->SetVisibility(true, true);
	CannonMesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> RotateMontageAsset(TEXT("/Script/Engine.AnimMontage'/Game/Minigun/Animations/A_Minigun/AM_Minigun-Cano_Rotation.AM_Minigun-Cano_Rotation'"));
	if (RotateMontageAsset.Succeeded())
	{
		CannonRotateMontage = RotateMontageAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find CannonRotateMontage asset!"));
	}
	
	bAiming = false;
	bShooting = false;
}

void ACHMinigun::Equip()
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
			CannonMesh1P->SetVisibility(true, true);
		}
		else
		{
			WeaponMesh1P->SetVisibility(false, true);
			CannonMesh1P->SetVisibility(false, true);
		}
	}
	
	if (WeaponMesh3P)
	{		
		WeaponMesh3P->AttachToComponent(OwningCharacter->GetMesh(), AttachmentRules, AttachPoint3P);
		WeaponMesh3P->CastShadow = true;
		WeaponMesh3P->bCastHiddenShadow = true;
		CannonMesh3P->CastShadow = true;
		CannonMesh3P->bCastHiddenShadow = true;
		
		if(OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First)
		{
			// WeaponMesh3P->SetVisibility(true, true); // Without this, the weapon's 3p shadow doesn't show
			WeaponMesh3P->SetVisibility(false, true);
			CannonMesh3P->SetVisibility(false, true);
		}
		else
		{
			WeaponMesh3P->SetVisibility(true, true);
			CannonMesh3P->SetVisibility(true, true);
		}
	}
	
	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
	{		
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		
		if(Subsystem->HasMappingContext(FireMappingContext))
		{
			UE_LOG(LogTemp, Log, TEXT("[ACHMinigun] Have FireMappingContext"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[ACHMinigun] No FireMappingContext"));
			Subsystem->AddMappingContext(FireMappingContext, 0);
			if(!OwningCharacter->GetbHasMinigunInputBindings())
			{
				SetupWeaponInputComponent();
				OwningCharacter->SetbHasMinigunInputBindings(true);
			}		
		}
	}
}

void ACHMinigun::UnEquip()
{
	Super::UnEquip();
	
	CannonMesh1P->SetVisibility(false, true);
	CannonMesh1P->CastShadow = false;
	CannonMesh3P->SetVisibility(false, true);
	CannonMesh3P->CastShadow = false;

	if (APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
			UE_LOG(LogTemp, Log, TEXT("[ACHMinigun] Removed %s"), *FireMappingContext->GetName());
		}
	}
}

void ACHMinigun::Fire()
{
	Super::Fire();
	// 쏘는 몽타주가 여기 있다. 총알이 다 차거나 재장전 중일 때 예외처리는 여기서 해야할 듯. 
	if(!bIsEquipped) return;
	if(bReloading || CurrentAmmoInClip <= 0) return;
	
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
	
	// LineTrace
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	// FString SocketName = "Muzzle_1";
	FTransform SocketTransform;
	if(OwningCharacter->IsInFirstPersonPerspective())
	{
		const USkeletalMeshSocket* MuzzleFlashSocket = CannonMesh1P->GetSocketByName("Muzzle_1");
		SocketTransform = MuzzleFlashSocket->GetSocketTransform(CannonMesh1P);
		if(MuzzleFlashSocket == nullptr) return; 
	}
	else
	{
		const USkeletalMeshSocket* MuzzleFlashSocket = CannonMesh3P->GetSocketByName("Muzzle_1");
		SocketTransform = MuzzleFlashSocket->GetSocketTransform(CannonMesh3P);
		if(MuzzleFlashSocket == nullptr) return; 
	}
	
	FVector TraceStart = SocketTransform.GetLocation();
	
	// FVector HitTarget = Hit.ImpactPoint;
	FVector End = Location + Rotation.Vector() * MaxRange;
	//FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;			// 연장선
	
	bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel4, Params);

	if (bSuccess)
	{
		// FVector ShotDirection = -Rotation.Vector();
		// DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10, FColor::Red, true);
		DrawDebugPoint(GetWorld(), Hit.Location, 10, FColor::Red, true);
		const float DamageToCause = Hit.BoneName.ToString() == FString("Head") ? HeadShotDamage : Damage;
		
		// 맞은 부위 효과
		if(ImpactEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation
			(
				GetWorld(),
				ImpactEffect,
				Hit.Location, 
				Hit.ImpactNormal.Rotation()
			);			
		}

		// AActor* HitActor = Hit.GetActor();
		ACHCharacterBase* CharacterBase = Cast<ACHCharacterBase>(Hit.GetActor());
		if (CharacterBase)
		{
			FPointDamageEvent DamageEvent(DamageToCause, Hit, Hit.ImpactNormal, nullptr);
			CharacterBase->TakeDamage(DamageToCause, DamageEvent, OwnerController, this);
		}
	}
	// 궤적
	FVector BeamEnd = End;
	if (Hit.bBlockingHit)
	{
		// BeamEnd = Hit.ImpactPoint;
		BeamEnd = Hit.Location;
	}
	else
	{
		Hit.Location = End;
	}
	
	if (TraceParticles)
	{
		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			TraceParticles,
			TraceStart,
			FRotator::ZeroRotator,
			true
		);
		if (Beam)
		{
			// Target은 그냥 임의로 지은 것.
			Beam->SetVectorParameter(FName("Target"), BeamEnd);
		}
	}
	
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			Hit.ImpactPoint
		);
	}
	
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			FireSound,
			GetActorLocation()
		);
	}

	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			MuzzleFlash,
			SocketTransform
		);
	}

	UAnimInstance* Weapon1pAnimInstance = WeaponMesh1P->GetAnimInstance();
	UAnimInstance* Weapon3pAnimInstance = WeaponMesh3P->GetAnimInstance();
	if(WeaponMeshFireMontage)
	{
		Weapon1pAnimInstance->Montage_Play(WeaponMeshFireMontage);
		Weapon3pAnimInstance->Montage_Play(WeaponMeshFireMontage);
	}
	
	// Get the animation object for the arms mesh
	UAnimInstance* TPAnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
	if (TPAnimInstance)
	{
		TPAnimInstance->Montage_Play(Fire3PMontage, 1);		
	}
	if(OwningCharacter->GetFirstPersonMesh())
	{
		UAnimInstance* FPAnimInstance = OwningCharacter->GetFirstPersonMesh()->GetAnimInstance();	
		if (FPAnimInstance)
		{
			FPAnimInstance->Montage_Play(Fire1PMontage, 1);
		}
	}
	
	if(!bInfiniteAmmo) CurrentAmmoInClip -= 1;	
}

void ACHMinigun::PullTrigger()
{
	Super::PullTrigger();
	if(!bIsEquipped) return;
	if(bReloading)
	{
		CancelPullTrigger();
		return;
	}

	bShooting = true;	
	OwningCharacter->bUseControllerRotationYaw = true;

	

	// 총 발사 Animation Montage
	if(CannonRotateMontage == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("CannonRotateMontage is null"));
		return;
	}
		
	if(CannonMesh1P == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("CannonMesh1P is null"));
		return;
	}
	
	if(CannonMesh3P == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("CannonMesh3P is null"));
		return;
	}
		
	UAnimInstance* Cannon1pAnimInstance = CannonMesh1P->GetAnimInstance();
	if(!Cannon1pAnimInstance->Montage_IsPlaying(CannonRotateMontage))
	{
		Cannon1pAnimInstance->Montage_Play(CannonRotateMontage, 1.f);				
	}

	UAnimInstance* Cannon3pAnimInstance = CannonMesh3P->GetAnimInstance();
	if(!Cannon3pAnimInstance->Montage_IsPlaying(CannonRotateMontage))
	{
		Cannon3pAnimInstance->Montage_Play(CannonRotateMontage, 1.f);				
	}
	
	
	// not aiming mode
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdAim
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdPrecisionAim
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::FirstScopeAim)
	{
		OwningCharacter->SetAiming(true);

		// play rotating cannon animation 
		GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHMinigun::Fire, FireInterval, true);
		
	}

	// 아직 조준하지 않은 상태. 
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdCover)
	{
		// hold a gun
		OwningCharacter->SetAiming(true);

		// holding a gun delay
		GetWorld()->GetTimerManager().SetTimer(ShootTimerHandle, [this]()
		{
			GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHMinigun::Fire, FireInterval, true);
		}, ShootingPreparationTime, false);	
		
	}
	bTrigger = true;
}

void ACHMinigun::CancelPullTrigger()
{
	Super::CancelPullTrigger();
	if(!bIsEquipped) return;

	bShooting = false;

	UAnimInstance* Weapon1pAnimInstance = WeaponMesh1P->GetAnimInstance();
	UAnimInstance* Weapon3pAnimInstance = WeaponMesh3P->GetAnimInstance();
	if(WeaponMeshFireMontage)
	{
		Weapon1pAnimInstance->Montage_Stop(2.0f,WeaponMeshFireMontage);
		Weapon3pAnimInstance->Montage_Stop(2.0f,WeaponMeshFireMontage);
	}
	
	if(bAiming == false)
	{
		// 총 발사 Animation Montage
		if(CannonRotateMontage == nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("CannonRotateMontage is null"));
			return;
		}
		
		if(CannonMesh1P == nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("CannonMesh1P is null"));
			return;
		}
	
		if(CannonMesh3P == nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("CannonMesh3P is null"));
			return;
		}
		
		UAnimInstance* Cannon1pAnimInstance = CannonMesh1P->GetAnimInstance();
		if(Cannon1pAnimInstance->Montage_IsPlaying(CannonRotateMontage))
		{
			Cannon1pAnimInstance->Montage_Stop(1.f, CannonRotateMontage);				
		}

		UAnimInstance* Cannon3pAnimInstance = CannonMesh3P->GetAnimInstance();
		if(Cannon3pAnimInstance->Montage_IsPlaying(CannonRotateMontage))
		{
			Cannon3pAnimInstance->Montage_Stop(1.f, CannonRotateMontage);				
		}
	}
	
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

void ACHMinigun::StartAim()
{
	Super::StartAim();
	if(!bIsEquipped) return;
	if(bReloading)
	{
		// cancel aim
		StopAim();
		return;
	}
	bAiming = true;

	if (CannonRotateSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			CannonRotateSound,
			GetActorLocation()
		);
	}
	
	// 총 발사 Animation Montage
	if(CannonRotateMontage == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("CannonRotateMontage is null"));
		return;
	}
		
	if(CannonMesh1P == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("CannonMesh1P is null"));
		return;
	}
	
	if(CannonMesh3P == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("CannonMesh3P is null"));
		return;
	}
		
	UAnimInstance* Cannon1pAnimInstance = CannonMesh1P->GetAnimInstance();
	if(!Cannon1pAnimInstance->Montage_IsPlaying(CannonRotateMontage))
	{
		Cannon1pAnimInstance->Montage_Play(CannonRotateMontage);
	}

	UAnimInstance* Cannon3pAnimInstance = CannonMesh3P->GetAnimInstance();
	if(!Cannon3pAnimInstance->Montage_IsPlaying(CannonRotateMontage))
	{
		Cannon3pAnimInstance->Montage_Play(CannonRotateMontage);				
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

	PlayerCharacter->SetMappingContextPriority(FireMappingContext, 2);
	
	// if Pull Triggering, pass
	if(!bTrigger) OwningCharacter->SetAiming(true);	
}

void ACHMinigun::StopAim()
{
	Super::StopAim();

	if(!bIsEquipped) return;
	// if(bReloading) return;

	bAiming = false;
	if(bShooting == false)
	{
		// UGameplayStatics::StopSou
		// 총 발사 Animation Montage
		if(CannonRotateMontage == nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("CannonRotateMontage is null"));
			return;
		}
		
		if(CannonMesh1P == nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("CannonMesh1P is null"));
			return;
		}
	
		if(CannonMesh3P == nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("CannonMesh3P is null"));
			return;
		}
		
		UAnimInstance* Cannon1pAnimInstance = CannonMesh1P->GetAnimInstance();
		if(Cannon1pAnimInstance->Montage_IsPlaying(CannonRotateMontage))
		{
			Cannon1pAnimInstance->Montage_Stop(1.f, CannonRotateMontage);				
		}

		UAnimInstance* Cannon3pAnimInstance = CannonMesh3P->GetAnimInstance();
		if(Cannon3pAnimInstance->Montage_IsPlaying(CannonRotateMontage))
		{
			Cannon3pAnimInstance->Montage_Stop(1.f, CannonRotateMontage);				
		}
	}	
	
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

void ACHMinigun::StartPrecisionAim()
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

	/*if(PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim)
	{
		PlayerCharacter->SetScopeAiming(true);
		PlayerCharacter->SetCharacterControl(ECharacterControlType::FirstScopeAim);
	}*/
}

void ACHMinigun::StopPrecisionAim()
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
	
	/*if(PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::FirstScopeAim)
	{
		PlayerCharacter->SetCharacterControl(ECharacterControlType::FirstAim);
		PlayerCharacter->SetScopeAiming(false);
	}*/
}

void ACHMinigun::Reload()
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

void ACHMinigun::SetupWeaponInputComponent()
{
	Super::SetupWeaponInputComponent();

	if (APlayerController* PlayerController = CastChecked<APlayerController>(OwningCharacter->GetController()))
	{
		// 무기를 가진 적이 있는지 확인하고 가지고 있으면 bind는 하지 않는다. 
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ACHMinigun::PullTrigger);	
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Canceled, this, &ACHMinigun::CancelPullTrigger);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ACHMinigun::StartAim);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Canceled, this, &ACHMinigun::StopAim);
			EnhancedInputComponent->BindAction(PrecisionAimAction, ETriggerEvent::Triggered, this, &ACHMinigun::StartPrecisionAim);
			EnhancedInputComponent->BindAction(CancelPrecisionAimAction, ETriggerEvent::Triggered, this, &ACHMinigun::StopPrecisionAim);
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ACHMinigun::Reload);
		}
	}
}

void ACHMinigun::SetWeaponMeshVisibility(uint8 bVisible)
{
	Super::SetWeaponMeshVisibility(bVisible);
	
}

void ACHMinigun::SetOwningCharacter(ACHCharacterBase* InOwningCharacter)
{
	Super::SetOwningCharacter(InOwningCharacter);
}

void ACHMinigun::PickUpOnTouch(ACHCharacterBase* InCharacter)
{
	Super::PickUpOnTouch(InCharacter);
}

void ACHMinigun::StopParticleSystem()
{
	Super::StopParticleSystem();
}
