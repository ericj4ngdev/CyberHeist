// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/CHGunRifle.h"
#include "AIController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/CHAnimInstance.h"
// #include "Character/CHCharacterBase.h"
#include "Character/CHCharacterPlayer.h"
#include "KisMet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/DamageEvents.h"
#include "Components/CapsuleComponent.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/CHPlayerController.h"
#include "CyberHeist.h"


ACHGunRifle::ACHGunRifle() 
{
	PrimaryActorTick.bCanEverTick = true;

	FireInterval = 0.1f;
	ReloadInterval = 2.f;
	ShootingPreparationTime = 0.2f;
	MaxRange = 10000.f;
	
	Damage = 15.f;
	HeadShotDamage = 100.0f;

	RecoilYaw = 0.2f;
	RecoilPitch = 0.43f;
	AimedRecoilYaw = 0.1f;
	AimedRecoilPitch = 0.15f;

	CurrentAmmoInClip = 30;
	ClipSize = 30;
	CurrentAmmo = 180;
	MaxAmmoCapacity = 999;
	bInfiniteAmmo = false;

	MuzzleCollision1P->SetRelativeLocation(FVector(0,50,10));
	MuzzleCollision1P->InitCapsuleSize(5.0f, 40.0f);

	MuzzleCollision3P->SetRelativeLocation(FVector(0,50,10));
	MuzzleCollision3P->InitCapsuleSize(5.0f, 40.0f);
}

void ACHGunRifle::BeginPlay()
{
	Super::BeginPlay();
	
	// ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);
	FTransform MuzzleSocketTransform_1P;
	FTransform HandleSocketTransform_1P;
	const USkeletalMeshSocket* MuzzleFlashSocket_1P = GetWeaponMesh1P()->GetSocketByName("MuzzleFlash");
	if(MuzzleFlashSocket_1P == nullptr) return;
	HandleSocket_1P = GetWeaponMesh1P()->GetSocketByName("Handle");
	MuzzleSocketTransform_1P = MuzzleFlashSocket_1P->GetSocketTransform(GetWeaponMesh1P());
	HandleSocketTransform_1P = HandleSocket_1P->GetSocketTransform(GetWeaponMesh1P());

	BarrelLength = FVector::Distance(MuzzleSocketTransform_1P.GetLocation(),HandleSocketTransform_1P.GetLocation()); 
	// CH_LOG(LogCHNetwork, Log, TEXT("1p : %s"),*SocketTransform.GetLocation().ToString())

	FTransform MuzzleSocketTransform_3P;
	FTransform HandleSocketTransform_3P;
	const USkeletalMeshSocket* MuzzleFlashSocket_3P = GetWeaponMesh3P()->GetSocketByName("MuzzleFlash");
	
	if(MuzzleFlashSocket_3P == nullptr) return; 
	HandleSocket_3P = GetWeaponMesh3P()->GetSocketByName("Handle");
	MuzzleSocketTransform_3P = MuzzleFlashSocket_3P->GetSocketTransform(GetWeaponMesh3P());
	HandleSocketTransform_3P = HandleSocket_3P->GetSocketTransform(GetWeaponMesh3P());
	// CH_LOG(LogCHNetwork, Log, TEXT("3p : %s"), *SocketTransform.GetLocation().ToString())

	CH_LOG(LogCHNetwork, Log, TEXT("Hand : %s Barrel : %f"), *HandleSocket_1P->GetSocketLocation(GetWeaponMesh1P()).ToString(), BarrelLength)
	
	if (HasAuthority())
	{
		// SetOwner();
		AActor* OwnerActor = GetOwner();
		if(OwnerActor)
		{
			CH_LOG(LogCHNetwork, Log, TEXT("Owner : %s"), *OwnerActor->GetName())
		}
		else
		{
			CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("No Owner"))
		}
	}
}

void ACHGunRifle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	
}

void ACHGunRifle::Equip()
{
	Super::Equip();

	if(!bIsEquipped) return;

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

	const USkeletalMeshSocket* HandSocket = OwningCharacter->GetMesh()->GetSocketByName(AttachPoint3P);
	if(HandSocket)
	{
		HandSocket->AttachActor(this,OwningCharacter->GetMesh());
		CH_LOG(LogCHNetwork, Log, TEXT("HandSocket: %s"), *HandSocket->GetSocketLocation(OwningCharacter->GetMesh()).ToString())
	}

	if (WeaponMesh1P)
	{
		WeaponMesh1P->AttachToComponent(OwningCharacter->GetFirstPersonMesh(), AttachmentRules, AttachPoint1P);		// 여기 npc가 총 들떄, 분명 null이라 에러 뜰텐데 
		WeaponMesh1P->SetRelativeRotation(FRotator(0, 0, -90.0f));
		if(!HasAuthority() && OwningCharacter->IsLocallyControlled())
		{
			if(OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First)
			{
				WeaponMesh1P->SetVisibility(true);
			}
			else
			{
				WeaponMesh1P->SetVisibility(false);
			}			
		}
		else
		{
			// 시점에 관계없이 남이 볼때는 1인칭 총은 무조건 안보이게 한다. 
			WeaponMesh1P->SetVisibility(false);
		}
	}
	
	if (WeaponMesh3P)
	{
		WeaponMesh3P->AttachToComponent(OwningCharacter->GetMesh(), AttachmentRules, AttachPoint3P);
		
		WeaponMesh3P->CastShadow = true;
		WeaponMesh3P->bCastHiddenShadow = true;
		if(!HasAuthority() && OwningCharacter->IsLocallyControlled())
		{
			if(OwningCharacter->CurrentCharacterControlType == ECharacterControlType::First)
			{
				WeaponMesh3P->SetVisibility(false);	
			}
			else
			{
				WeaponMesh3P->SetVisibility(true);
			}
		}
		else
		{
			// 서버는 무조건 3인칭 총 활성화
			WeaponMesh3P->SetVisibility(true);
		}		
	}
	
	CH_LOG(LogCHNetwork, Log, TEXT("AttachToComponent"))
	if(ACHCharacterPlayer* CHPlayer = Cast<ACHCharacterPlayer>(OwningCharacter))
	{
		MuzzleCollision1P->AttachToComponent(CHPlayer->GetFirstPersonCamera(),AttachmentRules);		// 1인칭을 들어오면 어태치가 안되는 건 아닌데... 
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
	
	if(OwningCharacter->IsLocallyControlled())
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
		{		
			UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());		
			if(Subsystem->HasMappingContext(FireMappingContext))
			{
				UE_LOG(LogTemp, Log, TEXT("[ACHGunRifle] Have FireMappingContext"));
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("[ACHGunRifle] No FireMappingContext"));
				Subsystem->AddMappingContext(FireMappingContext, 0);
				if(!OwningCharacter->GetbHasRifleInputBindings())
				{
					SetupWeaponInputComponent();
					OwningCharacter->SetbHasRifleInputBindings(true);
				}
			}
		}
	}
}

void ACHGunRifle::UnEquip()
{
	Super::UnEquip();

	DisableWeaponInput();
}

void ACHGunRifle::DisableWeaponInput()
{
	CH_LOG(LogCHNetwork, Log, TEXT("Begin"))
	Super::DisableWeaponInput();

	OwningCharacter->SetAiming(false);
	CancelPullTrigger();

	if (APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if(Subsystem->HasMappingContext(FireMappingContext))
			{
				Subsystem->RemoveMappingContext(FireMappingContext);
				CH_LOG(LogCHNetwork, Log, TEXT("[ACHGunRifle] Removed %s"), *FireMappingContext->GetName());				
			}
		}
	}
}

void ACHGunRifle::Fire()
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

	if(OwningCharacter->HasAuthority())
	{
		DrawDebugLine(GetWorld(),TraceStart, TraceEnd,FColor::Red,false, 2);		
	}
	if(bScreenLaserSuccess)
	{		
		DrawDebugPoint(GetWorld(), ScreenLaserHit.Location, 10, FColor::Red, false, 2);
	}
	else
	{
		// 허공에 쏴서 맞는 게 없다면
		CH_LOG(LogCHNetwork, Warning, TEXT("Sky"))
		ScreenLaserHit.Location = TraceEnd;
		DrawDebugPoint(GetWorld(), ScreenLaserHit.Location, 10, FColor::Red, false, 2);
	}
	// FVector HitLocation = bScreenLaserSuccess ? ScreenLaserHit.Location : TraceEnd;
	FVector HitLocation = ScreenLaserHit.Location;
	UE_LOG(LogTemp, Log, TEXT("HitLocation : %s "), *HitLocation.ToString());

	// 시작지점(총구)
	ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);
	FTransform SocketTransform;

	// 본인
	if(PlayerCharacter->IsInFirstPersonPerspective())
	{
		const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh1P()->GetSocketByName("MuzzleFlash");
		SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh1P());
		if(MuzzleFlashSocket == nullptr) return;
	}
	else
	{
		const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh3P()->GetSocketByName("MuzzleFlash");
		SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh3P());
		if(MuzzleFlashSocket == nullptr) return; 
	}	
	
	if(!OwningCharacter->HasAuthority())
	{
		LocalFire(HitLocation, SocketTransform);		// 이펙트만. 
	}
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh3P()->GetSocketByName("MuzzleFlash");
	SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh3P());
	if(MuzzleFlashSocket == nullptr) return; 
	ServerRPCFire(HitLocation, SocketTransform);
}

void ACHGunRifle::LocalFire(const FVector& HitLocation, const FTransform& MuzzleTransform)
{
	Super::LocalFire(HitLocation, MuzzleTransform);

	// 예외처리
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
	
	ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);
	
	// Muzzle LineTrace
	FHitResult MuzzleLaserHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	
	FVector MuzzleStart = MuzzleTransform.GetLocation();
	CH_LOG(LogCHNetwork, Log, TEXT("MuzzleStart : %s"), *MuzzleStart.ToString())
	
	FVector MuzzleEnd;
	MuzzleEnd = MuzzleStart + (HitLocation - MuzzleStart) * 1.25f;	
	CH_LOG(LogCHNetwork, Log, TEXT("MuzzleEnd : %s"), *MuzzleEnd.ToString())
	
	// 총구에서 레이저
	bool bMuzzleHit = GetWorld()->LineTraceSingleByChannel(MuzzleLaserHit, MuzzleStart, MuzzleEnd, ECollisionChannel::ECC_GameTraceChannel4, Params);

	// 최종 맞은 지점
	MuzzleLaserHit.Location = bMuzzleHit ? MuzzleLaserHit.Location : HitLocation;
	if(PlayerCharacter->HasAuthority())
	{
		// 그리는 건 서버에서만 그린다.... 
		DrawDebugLine(GetWorld(), MuzzleStart, MuzzleEnd, FColor::Blue, false, 2);
		DrawDebugPoint(GetWorld(), MuzzleLaserHit.Location, 20, FColor::Blue, false, 2);			
	}

	AController* OwnerController = OwnerPawn->GetController();		
	if (OwnerController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerController"));
	}
	
	// 서버
	if(OwnerController)
	{
		if(OwningCharacter->HasAuthority())
		{
			const float DamageToCause = MuzzleLaserHit.BoneName.ToString() == FString("Head") ? HeadShotDamage : Damage;
			CH_LOG(LogCHNetwork, Warning, TEXT("MuzzleLaserHit : %s "), *GetNameSafe(MuzzleLaserHit.GetActor()))
			
			AActor* HitActor = MuzzleLaserHit.GetActor();
			ACHCharacterBase* CharacterBase = Cast<ACHCharacterBase>(HitActor);
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
	CH_LOG(LogCHNetwork, Warning, TEXT("MuzzleLaserHit : %s "), *GetNameSafe(MuzzleLaserHit.GetActor()))

	// VFX
	{
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
				if(OwningCharacter->GetScopeAiming()) FPAnimInstance->Montage_Play(ScopeFire1PMontage,1);
				else FPAnimInstance->Montage_Play(Fire1PMontage, 1);
			}
		}
	}

	
	if(!bInfiniteAmmo) CurrentAmmoInClip -= 1;	
}

void ACHGunRifle::PullTriggerByAI(AActor* AttackTarget)
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
	FTimerDelegate TimerCallback = FTimerDelegate::CreateUObject(this, &ACHGunRifle::AutoFireByAI, AttackTarget);
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, TimerCallback, FireInterval, true);		
}

void ACHGunRifle::FireByAI(AActor* AttackTarget)
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
		const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh3P()->GetSocketByName("MuzzleFlash");
		SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh3P());
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
			
			// 맞은 부위 효과
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
		
		// 레이 쏘고 맞은 위치
		MulticastPlayFireVFX(HitTransform, SocketTransform);
		
		if(!bInfiniteAmmo) CurrentAmmoInClip -= 1;	
 			
		GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHGunRifle::EndShoot, FireInterval, false);	// End Attack.
	}	
}

void ACHGunRifle::AutoFireByAI(AActor* AttackTarget)
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
		const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh3P()->GetSocketByName("MuzzleFlash");
		SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh3P());
		if(MuzzleFlashSocket == nullptr) return; 
				
		FVector TraceStart = SocketTransform.GetLocation();
		//FVector End = Location + Rotation.Vector() * MaxRange;
		FVector End = AttackTarget->GetActorLocation();
		// 맞는 곳이 End이다.
		
		// FVector HitTarget = Hit.ImpactPoint;
		// FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;			// 연장선
		bool bSuccess = GetWorld()->LineTraceSingleByChannel(MuzzleLaserHit, TraceStart, End, ECollisionChannel::ECC_GameTraceChannel4, Params);
		UE_LOG(LogTemp, Log, TEXT("AttackTarget : %s , HitActor : %s"), *GetNameSafe(AttackTarget),*GetNameSafe(MuzzleLaserHit.GetActor()));
	
		if (bSuccess)
		{
			DrawDebugPoint(GetWorld(), MuzzleLaserHit.Location, 10, FColor::Red, true);
			const float DamageToCause = MuzzleLaserHit.BoneName.ToString() == FString("Head") ? HeadShotDamage : Damage;
			
			// 맞은 부위 효과
			/*if(ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation
				(
					GetWorld(),
					ImpactEffect,
					Hit.Location, 
					Hit.ImpactNormal.Rotation()
				);			
			}*/
	
			ACHCharacterPlayer* CharacterPlayer = Cast<ACHCharacterPlayer>(MuzzleLaserHit.GetActor());
			if (CharacterPlayer)
			{
				FPointDamageEvent DamageEvent(DamageToCause, MuzzleLaserHit, MuzzleLaserHit.ImpactNormal, nullptr);
				CharacterPlayer->TakeDamage(DamageToCause, DamageEvent, OwnerController, this);
			}
		}
		/*// 궤적
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
		}*/

		FTransform HitTransform;
		HitTransform.SetLocation(MuzzleLaserHit.Location);
		FVector ToTarget = MuzzleLaserHit.Location - SocketTransform.GetLocation();		
		HitTransform.SetRotation(ToTarget.Rotation().Quaternion());
		
		// 레이 쏘고 맞은 위치
		MulticastPlayFireVFX(HitTransform, SocketTransform);
		// 이거 서버는 애니메이션이 없어서 총이 그냥 바닥에 있는 듯..  
		
		/*
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
		*/	
		
		if(!bInfiniteAmmo) CurrentAmmoInClip -= 1;	
	}	
}

void ACHGunRifle::EndShoot()
{
	Super::EndShoot();

	
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
	if(OwningCharacter->GetNearWall()) return;
	
	OwningCharacter->bUseControllerRotationYaw = true;
	
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdAim
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdPrecisionAim
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim
		|| OwningCharacter->CurrentCharacterControlType == ECharacterControlType::FirstScopeAim)
	{
		if(FireMode == ECHFireMode::FullAuto)
		{
			GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHGunRifle::Fire, FireInterval, true);			
		}
		if(FireMode == ECHFireMode::SemiAuto)
		{
			Fire();					
		}
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
		// hold a gun. 총 들기만 하고 Aim은 아닌걸로		
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
			GetWorld()->GetTimerManager().SetTimer(ShootTimerHandle, [this]
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
	// AttackTargetActor = nullptr;
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third)
	{
		OwningCharacter->bUseControllerRotationYaw = false;
		// 리플리 하기
	}

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
	bHoldGun = false;
	ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);

	if(PlayerCharacter)
	{
		PlayerCharacter->SetMappingContextPriority(FireMappingContext, 2);
	
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
				PlayerCharacter->ServerRPC_SetCharacterControl(ECharacterControlType::FirstScopeAim);
			
				if(APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
				{
					PlayerController->SetViewTargetWithBlend(this,0.2f);
					OwningCharacter->GetFirstPersonMesh()->SetVisibility(false);	// 팔 보이게 하기
				}
			}
			else
			{			
				PlayerCharacter->ServerRPC_SetCharacterControl(ECharacterControlType::FirstAim);	
			}
		}
	}
	// CH_LOG(LogCHTemp, Log, TEXT("3p GetComponentLocation End : %s"), *WeaponMesh3P->GetComponentLocation().ToString())
	// if Pull Triggering, pass
	if(OwningCharacter->GetNearWall()) return;
	if(!bTrigger) OwningCharacter->SetAiming(true);	
}

void ACHGunRifle::StopAim()
{
	Super::StopAim();

	if(!bIsEquipped) return;
	// if(bReloading) return;
	bHoldGun = true;
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
				// PlayerCharacter->ReturnCover();			// 이것도 동기화.
				// PlayerCharacter->ServerSetActorRotation
				PlayerCharacter->ServerSetCoveredAttackMotion(false);	// 서버에서 return
				// PlayerCharacter->SetCoveredAttackMotion(false);			// 얘가 가장 먼저 실행됨... 해서 클라만 먼저 돈다. 그런데 제대로 못돈다... 
				// 이 후 LastCoveredRotation는 동기화됨
			}
			else
			{
				PlayerCharacter->ServerRPC_SetCharacterControl(ECharacterControlType::Third);			
			}
		}

		if (PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim)
		{
			PlayerCharacter->ServerRPC_SetCharacterControl(ECharacterControlType::First);
		}
	
		if(PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::FirstScopeAim)
		{
			PlayerCharacter->ServerRPC_SetCharacterControl(ECharacterControlType::First);
			if(APlayerController* PlayerController = CastChecked<APlayerController>(OwningCharacter->GetController()))
			{
				UE_LOG(LogTemp, Log, TEXT("SetViewTargetWithBlend"));
				PlayerController->SetViewTargetWithBlend(OwningCharacter,0.2f);
				OwningCharacter->GetFirstPersonMesh()->SetVisibility(true);		// 팔 보이게 하기	
			}
		}
	}
	if(!bTrigger)
	{
		OwningCharacter->SetAiming(false); // if PullTriggering, pass
	}
}

void ACHGunRifle::StartPrecisionAim()
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
	if(PlayerCharacter)
	{
		if(PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdAim)
		{
			// 조준경 bool 변수 -> 애니메이션에 전달
			PlayerCharacter->SetTPAimingCloser(true);
			// 카메라 위치 수정
			PlayerCharacter->ServerRPC_SetCharacterControl(ECharacterControlType::ThirdPrecisionAim);
		}

		if(PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim)
		{
			PlayerCharacter->SetScopeAiming(true);
			PlayerCharacter->ServerRPC_SetCharacterControl(ECharacterControlType::FirstScopeAim);
			if(ACHPlayerController* PlayerController = Cast<ACHPlayerController>(OwningCharacter->GetController()))
			{			
				PlayerController->SetViewTargetWithBlend(this,0.2);
				OwningCharacter->GetFirstPersonMesh()->SetVisibility(false);
			}
		}
	}
}

void ACHGunRifle::StopPrecisionAim()
{
	Super::StopPrecisionAim();
	// 휠 내리면 호출되는 함수
	if(!bIsEquipped) return;
	ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);
	if(PlayerCharacter)
	{
		if(PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::ThirdPrecisionAim)
		{
			PlayerCharacter->SetTPAimingCloser(false);
			PlayerCharacter->ServerRPC_SetCharacterControl(ECharacterControlType::ThirdAim);		
		}
	
		if(PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::FirstScopeAim)
		{
			PlayerCharacter->SetScopeAiming(false);
			OwningCharacter->GetFirstPersonMesh()->SetVisibility(true);		// 팔 보이게 하기
			if(ACHPlayerController* PlayerController = CastChecked<ACHPlayerController>(OwningCharacter->GetController()))
			{
				PlayerController->SetViewTargetWithBlend(OwningCharacter,0.2);
			}
			if(OwningCharacter->GetNearWall()) return;
			PlayerCharacter->ServerRPC_SetCharacterControl(ECharacterControlType::FirstAim);
		}
	}
}

void ACHGunRifle::StayPrecisionAim()
{
	Super::StayPrecisionAim();

	if(OwningCharacter->CurrentCharacterControlType == ECharacterControlType::FirstScopeAim)
	{
		if(ACHPlayerController* PlayerController = Cast<ACHPlayerController>(OwningCharacter->GetController()))
		{			
			PlayerController->SetViewTargetWithBlend(this,0.2);
			OwningCharacter->GetFirstPersonMesh()->SetVisibility(false);
		}
	}
}

void ACHGunRifle::Reload()
{
	LocalReload();
	Super::Reload();
	
}

void ACHGunRifle::LocalReload()
{
	Super::LocalReload();
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

	if(!OwningCharacter->HasAuthority())
	{
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
	}
	
	// 총 재장전 Animation Montage
	if(ReloadWeaponMontage)
	{
		WeaponMesh1P->GetAnimInstance()->Montage_Play(ReloadWeaponMontage);
		WeaponMesh3P->GetAnimInstance()->Montage_Play(ReloadWeaponMontage);				
	}
	
	// 플레이어 재장전 Animation Montage
	UAnimInstance* TPAnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
	if(TPAnimInstance) TPAnimInstance->Montage_Play(Reload3PMontage, 1);
	
	if(OwningCharacter->GetFirstPersonMesh())
	{
		UAnimInstance* FPAnimInstance = OwningCharacter->GetFirstPersonMesh()->GetAnimInstance();
		if(FPAnimInstance) FPAnimInstance->Montage_Play(Reload1PMontage,1);		
	}

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

void ACHGunRifle::SetupWeaponInputComponent()
{
	Super::SetupWeaponInputComponent();
	if(OwningCharacter)
	{
		if (APlayerController* PlayerController = CastChecked<APlayerController>(OwningCharacter->GetController()))
		{
			// 무기를 가진 적이 있는지 확인하고 가지고 있으면 bind는 하지 않는다. 
			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
			{
				EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ACHGunRifle::PullTrigger);	
				EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Canceled, this, &ACHGunRifle::CancelPullTrigger);
				EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ACHGunRifle::StartAim);
				EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ACHGunRifle::StopAim);
				EnhancedInputComponent->BindAction(PrecisionAimAction, ETriggerEvent::Triggered, this, &ACHGunRifle::StartPrecisionAim);
				EnhancedInputComponent->BindAction(CancelPrecisionAimAction, ETriggerEvent::Triggered, this, &ACHGunRifle::StopPrecisionAim);
				// EnhancedInputComponent->BindAction(FirstLookAction, ETriggerEvent::Triggered, this, &ACHGunRifle::FirstLook);
				EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ACHGunRifle::Reload);
			}
		}
	}
}

void ACHGunRifle::SetWeaponMeshVisibility(uint8 bVisible)
{
	Super::SetWeaponMeshVisibility(bVisible);
}

void ACHGunRifle::SetOwningCharacter(ACHCharacterBase* InOwningCharacter)
{
	Super::SetOwningCharacter(InOwningCharacter);
}

void ACHGunRifle::StopParticleSystem()
{
	Super::StopParticleSystem();
}

void ACHGunRifle::PlayFireVFX(const FTransform& HitTransform, const FTransform& MuzzleTransform)
{
	Super::PlayFireVFX(HitTransform, MuzzleTransform);

	if(ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation
		(
			GetWorld(),
			ImpactEffect,
			HitTransform			
			// Hit.Location, 
			//Hit.ImpactNormal.Rotation()			
		);			
	}

	// 궤적
	FVector BeamEnd = HitTransform.GetLocation();
	/*if (Hit.bBlockingHit)
	{
		// BeamEnd = Hit.ImpactPoint;
		BeamEnd = Hit.Location;
	}
	else
	{
		Hit.Location = End;
	}*/
		
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



