// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/CHMinigun.h"
#include "AIController.h"
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
#include "Kismet/KismetMathLibrary.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/CapsuleComponent.h"

ACHMinigun::ACHMinigun()
{
	PrimaryActorTick.bCanEverTick = true;
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
	
	FireInterval = 0.04f;
	ReloadInterval = 3.f;
	ShootingPreparationTime = 1.f;
	MaxRange = 10000.f;
	
	Damage = 15.f;
	HeadShotDamage = 100.0f;

	RecoilYaw = 0.2f;
	RecoilPitch = 0.43f;
	AimedRecoilYaw = 0.1f;
	AimedRecoilPitch = 0.15f;

	CurrentAmmoInClip = 1000;
	ClipSize = 1000;
	CurrentAmmo = 1000;
	MaxAmmoCapacity = 9999;
	bInfiniteAmmo = true;

}

void ACHMinigun::BeginPlay()
{
	Super::BeginPlay();
}

void ACHMinigun::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(OwningCharacter)
	{
		if(ACHCharacterPlayer* CHPlayer = Cast<ACHCharacterPlayer>(OwningCharacter))
		{
			// if(CHPlayer == nullptr) return;
			// 클라만 그리기
			if(OwningCharacter->HasAuthority()) return;
		
			if (MuzzleCollision1P)
			{
				// 캡슐의 위치와 방향 설정
				FVector CapsuleLocation = MuzzleCollision1P->GetComponentLocation();
				FRotator CapsuleRotation = MuzzleCollision1P->GetComponentRotation();

				// 캡슐의 반지름과 높이 설정
				float CapsuleRadius = MuzzleCollision1P->GetScaledCapsuleRadius();
				float CapsuleHalfHeight = MuzzleCollision1P->GetScaledCapsuleHalfHeight();

				// Trace 시작점과 끝점 설정
				FVector Start = CapsuleLocation - FVector(0, 0, CapsuleHalfHeight);
				FVector End = CapsuleLocation + FVector(0, 0, CapsuleHalfHeight);
		
				FHitResult HitResult;
				FCollisionQueryParams Params(FName(TEXT("Cover")), true, this);
				Params.AddIgnoredActor(this);
				Params.AddIgnoredActor(GetOwner());

				bool HitDetected = GetWorld()->SweepSingleByChannel(HitResult, Start, End,FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(CapsuleRadius), Params);
				
				FColor DrawColor = HitDetected ? FColor::Green : FColor::Blue;
				// Debug 캡슐 그리기
				DrawDebugCapsule(GetWorld(), CapsuleLocation, CapsuleHalfHeight, CapsuleRadius, CapsuleRotation.Quaternion(), DrawColor);
			}
			if (MuzzleCollision3P)
			{
				// 캡슐의 위치와 방향 설정
				FVector CapsuleLocation = MuzzleCollision3P->GetComponentLocation();
				FRotator CapsuleRotation = MuzzleCollision3P->GetComponentRotation();

				// 캡슐의 반지름과 높이 설정
				float CapsuleRadius = MuzzleCollision3P->GetScaledCapsuleRadius();
				float CapsuleHalfHeight = MuzzleCollision3P->GetScaledCapsuleHalfHeight();

				// Trace 시작점과 끝점 설정
				FVector Start = CapsuleLocation - FVector(0, 0, CapsuleHalfHeight);
				FVector End = CapsuleLocation + FVector(0, 0, CapsuleHalfHeight);
		
				FHitResult HitResult;
				FCollisionQueryParams Params(FName(TEXT("Cover")), true, this);
				Params.AddIgnoredActor(this);
				Params.AddIgnoredActor(GetOwner());
				
				bool HitDetected = GetWorld()->SweepSingleByChannel(HitResult, Start, End,FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(CapsuleRadius), Params);
				
				FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
				// Debug 캡슐 그리기
				DrawDebugCapsule(GetWorld(), CapsuleLocation, CapsuleHalfHeight, CapsuleRadius, CapsuleRotation.Quaternion(), DrawColor);
			}
		}
	}
}

void ACHMinigun::Equip()
{
	if(ACHCharacterPlayer* CHPlayer = Cast<ACHCharacterPlayer>(OwningCharacter))
	{
		if(CHPlayer->bIsCrouched) return;
	}
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

		if(!HasAuthority() && OwningCharacter->IsLocallyControlled())
		{
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
		if(!HasAuthority() && OwningCharacter->IsLocallyControlled())
		{
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
		else
		{
			WeaponMesh3P->SetVisibility(true, true);
			CannonMesh3P->SetVisibility(true, true);
		}
	}

	if(ACHCharacterPlayer* CHPlayer = Cast<ACHCharacterPlayer>(OwningCharacter))
	{
		MuzzleCollision1P->AttachToComponent(CHPlayer->GetFirstPersonCamera(),AttachmentRules);
		MuzzleCollision3P->AttachToComponent(CHPlayer->GetThirdPersonCamera(), AttachmentRules);

		// GetThirdPersonCamera의 위치와 회전을 가져옵니다.
		const FVector CameraLocation1P = CHPlayer->GetFirstPersonCamera()->GetComponentLocation();
		const FRotator CameraRotation1P = CHPlayer->GetFirstPersonCamera()->GetComponentRotation();
		const FVector CameraLocation3P = CHPlayer->GetThirdPersonCamera()->GetComponentLocation();
		const FRotator CameraRotation3P = CHPlayer->GetThirdPersonCamera()->GetComponentRotation();

		// 카메라의 방향 벡터를 계산합니다.
		const FVector CameraForwardVector1P = CameraRotation1P.Vector();
		const FVector CameraForwardVector3P = CameraRotation3P.Vector();

		// 새로운 위치를 계산합니다.
		const FVector MuzzleCapsuleLocation1P = CameraLocation1P + (CameraForwardVector1P * 120);
		const FVector MuzzleCapsuleLocation3P = CameraLocation3P + (CameraForwardVector3P * 120);
		MuzzleCollision1P->SetWorldLocation(MuzzleCapsuleLocation1P);
		MuzzleCollision3P->SetWorldLocation(MuzzleCapsuleLocation3P);

		// 카메라의 회전에 90도 회전 (예: Y축 기준) 추가
		const FRotator MuzzleCapsuleRotation1P = CameraRotation1P + FRotator(90.0f, 0.0f, 0.0f); // Y축을 기준으로 90도 회전
		const FRotator MuzzleCapsuleRotation3P = CameraRotation3P + FRotator(90.0f, 0.0f, 0.0f); // Y축을 기준으로 90도 회전

		// 캡슐의 회전을 조정된 회전으로 설정합니다.
		MuzzleCollision1P->SetWorldRotation(MuzzleCapsuleRotation1P);
		MuzzleCollision3P->SetWorldRotation(MuzzleCapsuleRotation3P);
	}
	
	// Set up action bindings
	if(OwningCharacter->IsLocallyControlled())
	{
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
}

void ACHMinigun::UnEquip()
{
	Super::UnEquip();
	
	CannonMesh1P->SetVisibility(false, true);
	CannonMesh1P->CastShadow = false;
	CannonMesh3P->SetVisibility(false, true);
	CannonMesh3P->CastShadow = false;

	DisableWeaponInput();	
}

void ACHMinigun::Fire()
{
	Super::Fire();
	AController* OwnerController = OwningCharacter->GetController();		
	
	if (OwnerController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerController"));
		return;
	}
	// Viewport LineTrace
	FHitResult ScreenLaserHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	
	FVector TraceStart;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(TraceStart, Rotation);
	// DrawDebugCamera(GetWorld(), Location, Rotation, 90, 2, FColor::Red, true);
	FVector TraceEnd = TraceStart + Rotation.Vector() * MaxRange;
	bool bScreenLaserSuccess = GetWorld()->LineTraceSingleByChannel(ScreenLaserHit, TraceStart, TraceEnd, ECollisionChannel::ECC_GameTraceChannel4, Params);
	DrawDebugLine(GetWorld(),TraceStart, TraceEnd,FColor::Red,false, 2);
	DrawDebugPoint(GetWorld(), ScreenLaserHit.Location, 10, FColor::Red, false, 2);
	
	FVector HitLocation = bScreenLaserSuccess ? ScreenLaserHit.Location : TraceEnd;
	UE_LOG(LogTemp, Log, TEXT("HitLocation : %s "), *HitLocation.ToString());

	// 시작지점(총구)
	ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);
	FTransform SocketTransform;
	if(PlayerCharacter->IsInFirstPersonPerspective())
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
	if(!OwningCharacter->HasAuthority())
	{
		LocalFire(HitLocation, SocketTransform);		// 이펙트만. 
	}
	ServerRPCFire(HitLocation, SocketTransform);
}

void ACHMinigun::DisableWeaponInput()
{
	Super::DisableWeaponInput();

	OwningCharacter->SetAiming(false);
	CancelPullTrigger();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
			UE_LOG(LogTemp, Log, TEXT("[ACHMinigun] Removed %s"), *FireMappingContext->GetName());
		}
	}
}

void ACHMinigun::LocalFire(const FVector& HitLocation, const FTransform& MuzzleTransform)
{
	Super::LocalFire(HitLocation, MuzzleTransform);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerPawn"));
		return;
	}
	
	// Recoil
	if(bHoldGun)
	{
		UE_LOG(LogTemp, Log, TEXT("Not Aim"));
		float RYaw = UKismetMathLibrary::RandomFloatInRange(-RecoilYaw, RecoilYaw);
		float RPitch = UKismetMathLibrary::RandomFloatInRange(-RecoilPitch,0.0f);
		OwnerPawn->AddControllerYawInput(RYaw);
		OwnerPawn->AddControllerPitchInput(RPitch);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Aim"));
		float RYaw = UKismetMathLibrary::RandomFloatInRange(-AimedRecoilYaw, AimedRecoilYaw);
		float RPitch = UKismetMathLibrary::RandomFloatInRange(-AimedRecoilPitch,0.0f);
		OwnerPawn->AddControllerYawInput(RYaw);
		OwnerPawn->AddControllerPitchInput(RPitch);
	}
	

	// Muzzle LineTrace
	FHitResult MuzzleLaserHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	FVector MuzzleStart = MuzzleTransform.GetLocation();
	FVector MuzzleEnd;
	MuzzleEnd = MuzzleStart + (HitLocation - MuzzleStart) * 1.25f;	
	
	
	// 총구에서 레이저
	GetWorld()->LineTraceSingleByChannel(MuzzleLaserHit, MuzzleStart, MuzzleEnd, ECollisionChannel::ECC_GameTraceChannel4);
	ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);
	if(PlayerCharacter->HasAuthority())
	{
		DrawDebugLine(GetWorld(), MuzzleStart, MuzzleEnd, FColor::Blue, false, 2);
		DrawDebugPoint(GetWorld(), MuzzleLaserHit.Location, 10, FColor::Blue, false, 2);
	}
	
	AController* OwnerController = OwnerPawn->GetController();
	if (OwnerController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerController"))
	}
	
	if(OwnerController)
	{
		if(OwningCharacter->HasAuthority())
		{
			const float DamageToCause = MuzzleLaserHit.BoneName.ToString() == FString("Head") ? HeadShotDamage : Damage;
			UE_LOG(LogTemp, Log, TEXT("MuzzleLaserHit : %s "), *GetNameSafe(MuzzleLaserHit.GetActor()))
			AActor* HitActor = MuzzleLaserHit.GetActor();
			ACHCharacterBase* CharacterBase = Cast<ACHCharacterBase>(MuzzleLaserHit.GetActor());
			if (CharacterBase)
			{
				FPointDamageEvent DamageEvent(DamageToCause, MuzzleLaserHit, MuzzleLaserHit.ImpactNormal, nullptr);
				CharacterBase->TakeDamage(DamageToCause, DamageEvent, OwnerController, this);
			}
			else
			{
				UGameplayStatics::ApplyDamage(HitActor,DamageToCause,OwnerController,this,UDamageType::StaticClass());
				UE_LOG(LogTemp, Log, TEXT("HitActor : %s"), *GetNameSafe(HitActor))
			}
		}
	}


	// 위치가 같다면 정중앙 효과
	// 다르면 총구 레이저 효과
	if(ImpactEffect)
	{
		if(MuzzleLaserHit.Location.Equals(HitLocation))
		{
		
			UGameplayStatics::SpawnEmitterAtLocation
			(
				GetWorld(),
				ImpactEffect,
				HitLocation, 
				MuzzleLaserHit.ImpactNormal.Rotation()
			);			
		
		}
		else
		{
			UGameplayStatics::SpawnEmitterAtLocation
			(
				GetWorld(),
				ImpactEffect,
				MuzzleLaserHit.Location, 
				MuzzleLaserHit.ImpactNormal.Rotation()
			);			
		
		}
	}
	
	// 궤적
	FVector BeamEnd = HitLocation;
	if (MuzzleLaserHit.bBlockingHit)
	{
		// BeamEnd = Hit.ImpactPoint;
		BeamEnd = MuzzleLaserHit.Location;
	}
	else
	{
		MuzzleLaserHit.Location = HitLocation;
	}
	
	if (TraceParticles)
	{
		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			TraceParticles,
			MuzzleStart,
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
			MuzzleLaserHit.ImpactPoint
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
			MuzzleTransform
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

void ACHMinigun::PullTriggerByAI(AActor* AttackTarget)
{
	Super::PullTriggerByAI(AttackTarget);
	if(bReloading)
	{
		CancelPullTrigger();
		return;
	}
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
	FireByAI(AttackTarget);
	FTimerDelegate TimerCallback = FTimerDelegate::CreateUObject(this, &ACHMinigun::AutoFireByAI, AttackTarget);
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, TimerCallback, FireInterval, true);		
}

void ACHMinigun::FireByAI(AActor* AttackTarget)
{
	Super::FireByAI(AttackTarget);
	
	if(OwningCharacter)
	{
		OwningCharacter->SetAiming(true);

		if(!bIsEquipped) return;
		if(bReloading)
		{
			CancelPullTrigger();
			return;
		}
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
		
		OwningCharacter->SetIsAttacking(true);
	
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
			
		// LineTrace
		FHitResult MuzzleLaserHit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(GetOwner());
			
		FTransform SocketTransform;
		const USkeletalMeshSocket* MuzzleFlashSocket = CannonMesh3P->GetSocketByName("Muzzle_1");
		SocketTransform = MuzzleFlashSocket->GetSocketTransform(CannonMesh3P);
		if(MuzzleFlashSocket == nullptr) return; 
				
		FVector TraceStart = SocketTransform.GetLocation();
		//FVector End = Location + Rotation.Vector() * MaxRange;
		FVector End = AttackTarget->GetActorLocation();
		//FVector HitTarget = Hit.ImpactPoint;
		
		// FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;			// 연장선
		bool bSuccess = GetWorld()->LineTraceSingleByChannel(MuzzleLaserHit, Location, End, ECollisionChannel::ECC_GameTraceChannel4, Params);
		UE_LOG(LogTemp, Log, TEXT("AttackTarget : %s , HitActor : %s"), *GetNameSafe(AttackTarget),*GetNameSafe(MuzzleLaserHit.GetActor()));
	
		if (bSuccess)
		{
			// FVector ShotDirection = -Rotation.Vector();
			// DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10, FColor::Red, true);
			DrawDebugPoint(GetWorld(), MuzzleLaserHit.Location, 10, FColor::Red, true);
			const float DamageToCause = MuzzleLaserHit.BoneName.ToString() == FString("Head") ? HeadShotDamage : Damage;
			
			ACHCharacterPlayer* CharacterPlayer = Cast<ACHCharacterPlayer>(MuzzleLaserHit.GetActor());
			if (CharacterPlayer)
			{
				FPointDamageEvent DamageEvent(DamageToCause, MuzzleLaserHit, MuzzleLaserHit.ImpactNormal, nullptr);
				CharacterPlayer->TakeDamage(DamageToCause, DamageEvent, OwnerController, this);
			}
		}

		FTransform HitTransform;
		HitTransform.SetLocation(MuzzleLaserHit.Location);
		FVector ToTarget = MuzzleLaserHit.Location - SocketTransform.GetLocation();		
		HitTransform.SetRotation(ToTarget.Rotation().Quaternion());
		
		MulticastPlayFireVFX(HitTransform, SocketTransform);
		
		if(!bInfiniteAmmo) CurrentAmmoInClip -= 1;	
 			
		GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHMinigun::EndShoot, FireInterval, false);	// End Attack.
	}
}

void ACHMinigun::AutoFireByAI(AActor* AttackTarget)
{
	Super::AutoFireByAI(AttackTarget);

	if(OwningCharacter)
	{
		OwningCharacter->SetAiming(true);

		if(!bIsEquipped) return;
		if(bReloading || CurrentAmmoInClip <= 0) return;
		OwningCharacter->SetIsAttacking(true);
		// return;
	
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
			
		// LineTrace
		FHitResult MuzzleLaserHit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(GetOwner());
			
		FTransform SocketTransform;
		// const USkeletalMeshSocket* MuzzleFlashSocket = CannonMesh3P->GetSocketByName("MuzzleFlash");
		const USkeletalMeshSocket* MuzzleFlashSocket = CannonMesh3P->GetSocketByName("Muzzle_1");
		SocketTransform = MuzzleFlashSocket->GetSocketTransform(CannonMesh3P);
		if(MuzzleFlashSocket == nullptr) return; 
				
		FVector TraceStart = SocketTransform.GetLocation();
		//FVector End = Location + Rotation.Vector() * MaxRange;
		FVector End = AttackTarget->GetActorLocation();
		//FVector HitTarget = Hit.ImpactPoint;
		// FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;			// 연장선
		bool bSuccess = GetWorld()->LineTraceSingleByChannel(MuzzleLaserHit, TraceStart, End, ECollisionChannel::ECC_GameTraceChannel4, Params);
		UE_LOG(LogTemp, Log, TEXT("AttackTarget : %s , HitActor : %s"), *GetNameSafe(AttackTarget),*GetNameSafe(MuzzleLaserHit.GetActor()));
	
		if (bSuccess)
		{
			// FVector ShotDirection = -Rotation.Vector();
			// DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10, FColor::Red, true);
			DrawDebugPoint(GetWorld(), MuzzleLaserHit.Location, 10, FColor::Red, true);
			const float DamageToCause = MuzzleLaserHit.BoneName.ToString() == FString("Head") ? HeadShotDamage : Damage;

			ACHCharacterPlayer* CharacterPlayer = Cast<ACHCharacterPlayer>(MuzzleLaserHit.GetActor());
			if (CharacterPlayer)
			{
				FPointDamageEvent DamageEvent(DamageToCause, MuzzleLaserHit, MuzzleLaserHit.ImpactNormal, nullptr);
				CharacterPlayer->TakeDamage(DamageToCause, DamageEvent, OwnerController, this);
			}
		}

		FTransform HitTransform;
		HitTransform.SetLocation(MuzzleLaserHit.Location);
		FVector ToTarget = MuzzleLaserHit.Location - SocketTransform.GetLocation();		
		HitTransform.SetRotation(ToTarget.Rotation().Quaternion());

		MulticastPlayFireVFX(HitTransform, SocketTransform);
		
		/*UAnimInstance* Weapon3pAnimInstance = WeaponMesh3P->GetAnimInstance();
		if(WeaponMeshFireMontage)
		{
			Weapon3pAnimInstance->Montage_Play(WeaponMeshFireMontage);
		}
		
		// Get the animation object for the arms mesh
		UAnimInstance* TPAnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
		if (TPAnimInstance)
		{
			TPAnimInstance->Montage_Play(Fire3PMontage, 1);		
		}*/		
		
		if(!bInfiniteAmmo) CurrentAmmoInClip -= 1;	
	}
}

void ACHMinigun::EndShoot()
{
	Super::EndShoot();

	UAnimInstance* Weapon3pAnimInstance = WeaponMesh3P->GetAnimInstance();
	if(WeaponMeshFireMontage)
	{
		Weapon3pAnimInstance->Montage_Stop(2.0f,WeaponMeshFireMontage);
	}
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

	if(OwningCharacter->GetNearWall()) return;
	
	bShooting = true;	
	OwningCharacter->SetIsAttacking(bShooting);
	// OwningCharacter->bUseControllerRotationYaw = true;

	// 총 발사 Animation Montage
	// 여기 ServerRotateCannon()
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
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim)
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
		if(OwningCharacter->GetNearWall())
		{
			CancelPullTrigger();
			return;
		}
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
	if(!bIsEquipped) return;
	Super::CancelPullTrigger();			// 서버 RPC보내기

	bShooting = false;
	OwningCharacter->SetIsAttacking(bShooting);
	
	// 서버에서 동기화가 안됨... 
	// if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third)
	// 		OwningCharacter->bUseControllerRotationYaw = false;	

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
	if(!bIsEquipped) return;
	if(bReloading)
	{
		// cancel aim
		StopAim();
		return;
	}
	Super::StartAim();
	bHoldGun = false;
	// bAiming = true;

	
	
	// 총 발사 Animation Montage
	/*if(CannonRotateMontage == nullptr)
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
	}*/
	
	ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);

	if(PlayerCharacter)
	{
		if (PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::Third)
		{
			PlayerCharacter->ServerRPC_SetCharacterControl(ECharacterControlType::ThirdAim);
		}

		if(PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdCover)
		{
			if(!PlayerCharacter->GetCovered())
			{
				UE_LOG(LogTemp,Warning,TEXT("Cover variable is not correct"));
			}		
			PlayerCharacter->ServerRPC_SetCharacterControl(ECharacterControlType::ThirdAim);
			PlayerCharacter->SetCoveredAttackMotion(true);
			PlayerCharacter->ServerSetCoveredAttackMotion(true);
		}

		if (PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::First)
		{
			// 조준 여부 변수 추가
			// PlayerCharacter->SetAiming(true);  // 근데 이건 밑에 코드에서 해줌
			if(PlayerCharacter->GetScopeAiming())
			{
				// 미니건은 불가
				PlayerCharacter->SetScopeAiming(false);
			}
			PlayerCharacter->ServerRPC_SetCharacterControl(ECharacterControlType::FirstAim);
			
		}
		PlayerCharacter->SetMappingContextPriority(FireMappingContext, 2);
	}
	
	// if Pull Triggering, pass
	if(OwningCharacter->GetNearWall()) return;
	if(!bTrigger) OwningCharacter->SetAiming(true);	
}

void ACHMinigun::StopAim()
{
	if(!bIsEquipped) return;
	// if(bReloading) return;
	Super::StopAim();

	bHoldGun = true;
	// bAiming = false;
	if(bShooting == false)
	{
		UE_LOG(LogTemp, Log, TEXT("StopAim"));
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

	if(PlayerCharacter)
	{
		PlayerCharacter->SetMappingContextPriority(FireMappingContext, 0);
	
		if (PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdAim
			|| PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdPrecisionAim)
		{
			if(PlayerCharacter->GetCovered())
			{
				PlayerCharacter->ServerRPC_SetCharacterControl(ECharacterControlType::ThirdCover);
				PlayerCharacter->SetCoveredAttackMotion(false);			
			}
			else
			{
				PlayerCharacter->ServerRPC_SetCharacterControl(ECharacterControlType::Third);			
			}
		}
		
		if (PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim)
		{		
			PlayerCharacter->ServerRPC_SetCharacterControl(ECharacterControlType::First);
			// 상태만 바꾸는 것 같지만 조준 변수는 밑에서 조정
		}
	}
	if(!bTrigger)
	{
		OwningCharacter->SetAiming(false); // if PullTriggering, pass
	}
}

void ACHMinigun::StartPrecisionAim()
{
	Super::StartPrecisionAim();
	// 휠 올리면 호출되는 함수
	bHoldGun = false;
}

void ACHMinigun::StopPrecisionAim()
{
	Super::StopPrecisionAim();
	// 휠 내리면 호출되는 함수	
}

void ACHMinigun::StayPrecisionAim()
{
	Super::StayPrecisionAim();
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

	if(ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter))
	{
		// 조준 중이라면 해제
		if(PlayerCharacter->IsInFirstPersonPerspective())
		{
			PlayerCharacter->ServerRPC_SetCharacterControl(ECharacterControlType::First);
		}
		else
		{
			// 엄폐는 유지
			if(PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdCover)
			{
				PlayerCharacter->ServerRPC_SetCharacterControl(ECharacterControlType::ThirdCover);			
			}
			else
			{
				PlayerCharacter->ServerRPC_SetCharacterControl(ECharacterControlType::Third);			
			}
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
	if(OwningCharacter)
	{
		if (APlayerController* PlayerController = CastChecked<APlayerController>(OwningCharacter->GetController()))
		{
			// 무기를 가진 적이 있는지 확인하고 가지고 있으면 bind는 하지 않는다. 
			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
			{
				EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ACHMinigun::PullTrigger);	
				EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Canceled, this, &ACHMinigun::CancelPullTrigger);
				EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ACHMinigun::StartAim);
				EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ACHMinigun::StopAim);
				EnhancedInputComponent->BindAction(PrecisionAimAction, ETriggerEvent::Triggered, this, &ACHMinigun::StartPrecisionAim);
				EnhancedInputComponent->BindAction(CancelPrecisionAimAction, ETriggerEvent::Triggered, this, &ACHMinigun::StopPrecisionAim);
				// EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ACHMinigun::Reload);
			}
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

void ACHMinigun::StopParticleSystem()
{
	Super::StopParticleSystem();
}

void ACHMinigun::OnStartAim()
{
	Super::OnStartAim();
	StartRotateCannon();
	if (CannonRotateSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			CannonRotateSound,
			GetActorLocation()
		);
	}
}

void ACHMinigun::OnStopAim()
{
	Super::OnStopAim();
	StopRotateCannon();
}

void ACHMinigun::OnPullTrigger()
{
	Super::OnPullTrigger();
	StartRotateCannon();
}

void ACHMinigun::OnCancelPullTrigger()
{
	Super::OnCancelPullTrigger();
	StopRotateCannon();
}

void ACHMinigun::StartRotateCannon()
{
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
}

void ACHMinigun::StopRotateCannon()
{
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
}

void ACHMinigun::PlayFireVFX(const FTransform& HitTransform, const FTransform& MuzzleTransform)
{
	Super::PlayFireVFX(HitTransform, MuzzleTransform);
	
	if(ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation
		(
			GetWorld(),
			ImpactEffect,
			HitTransform
		);			
	}

	// 궤적
	FVector BeamEnd = HitTransform.GetLocation();
		
	if (TraceParticles)
	{
		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			TraceParticles,
			MuzzleTransform.GetLocation(),
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
			HitTransform.GetLocation()
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
			MuzzleTransform
		);
	}

	UAnimInstance* Weapon3pAnimInstance = WeaponMesh3P->GetAnimInstance();
	if(WeaponMeshFireMontage)
	{
		Weapon3pAnimInstance->Montage_Play(WeaponMeshFireMontage);
	}
		
	// Get the animation object for the arms mesh
	UAnimInstance* TPAnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
	if (TPAnimInstance)
	{
		TPAnimInstance->Montage_Play(Fire3PMontage, 1);		
	}	
}