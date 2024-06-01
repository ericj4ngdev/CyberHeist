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

	MuzzleCollision->SetRelativeLocation(FVector(0,80,10));
	MuzzleCollision->InitCapsuleSize(5.0f, 5.0f);
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

	CH_LOG(LogCHTemp, Log, TEXT("Hand : %s Barrel : %f"), *HandleSocket_1P->GetSocketLocation(GetWeaponMesh1P()).ToString(), BarrelLength)
	
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
	}
	
	if (WeaponMesh1P)
	{
		AttachToComponent(OwningCharacter->GetFirstPersonMesh(), AttachmentRules, AttachPoint1P);		// 여기 npc가 총 들떄, 분명 null이라 에러 뜰텐데 
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
		WeaponMesh3P->bCastHiddenShadow = true;

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

	// 로컬
	if(!HasAuthority())
	{
		AController* OwnerController = OwningCharacter->GetController();		
		if (OwnerController == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("OwnerController"));
			return;
		}
		// Viewport LineTrace	
		/*FVector TraceStart;
		FRotator Rotation;
		OwnerController->GetPlayerViewPoint(TraceStart, Rotation);
		DrawDebugCamera(GetWorld(), TraceStart, Rotation, 90, 2, FColor::Red, false,2);
		FVector TraceEnd = TraceStart + Rotation.Vector() * 1000.f;
	
		// 손잡이 위치 
		FVector Direction = HandleSocket_1P->GetSocketLocation(GetWeaponMesh1P()) - TraceEnd;
		// CH_LOG(LogTemp, Log, TEXT("Hand : %s Barrel : %f"), *HandleSocket_1P->GetSocketLocation(GetWeaponMesh1P()).ToString(), BarrelLength)
		MuzzleCollision->SetRelativeLocation(HandleSocket_3P->GetSocketLocation(GetWeaponMesh3P()) + BarrelLength * TraceEnd);*/
		// MuzzleCollision->SetWorldLocation()

		FVector TraceStart;
		FRotator Rotation;
		OwnerController->GetPlayerViewPoint(TraceStart, Rotation);
		DrawDebugCamera(GetWorld(), TraceStart, Rotation, 90, 2, FColor::Red, false,2);
		FVector TraceEnd = TraceStart + Rotation.Vector() * 1000.f;

		FVector Direction = HandleSocket_3P->GetSocketLocation(GetWeaponMesh3P()) - TraceEnd;
		// 내분점
	
		DrawDebugPoint(GetWorld(),TraceEnd,10.0f,FColor::Magenta,false,2);
		// 3인칭 메시 기준 
		DrawDebugLine(GetWorld(),HandleSocket_3P->GetSocketLocation(GetWeaponMesh3P()),TraceEnd,FColor::Magenta,false,2);
		MuzzleCollision->SetWorldLocation(HandleSocket_3P->GetSocketLocation(GetWeaponMesh3P()) + Direction.GetSafeNormal() * BarrelLength * 2);
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
	if(OwningCharacter->IsLocallyControlled())
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				if(Subsystem->HasMappingContext(FireMappingContext))
				{
					Subsystem->RemoveMappingContext(FireMappingContext);
					UE_LOG(LogTemp, Log, TEXT("[ACHGunRifle] Removed %s"), *FireMappingContext->GetName());				
				}
			}
		}
	}
}

void ACHGunRifle::Fire()
{
	Super::Fire();	
}

void ACHGunRifle::LocalFire(const FVector& HitLocation,const FVector& TraceEnd)
{
	Super::LocalFire(HitLocation, TraceEnd);	

	// 예외처리
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerPawn"));
		return;
	}

	// Recoil
	{
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
	}
	ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);
	// Socket
	FTransform SocketTransform;	
	if(PlayerCharacter->IsInFirstPersonPerspective())
	{
		const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh1P()->GetSocketByName("MuzzleFlash");
		SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh1P());
		if(MuzzleFlashSocket == nullptr) return;
		CH_LOG(LogCHNetwork, Log, TEXT("1p : %s"),*SocketTransform.GetLocation().ToString())
	}
	else
	{
		const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh3P()->GetSocketByName("MuzzleFlash");
		SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh3P());
		if(MuzzleFlashSocket == nullptr) return; 
		CH_LOG(LogCHNetwork, Log, TEXT("3p : %s"), *SocketTransform.GetLocation().ToString())
	}
	
	
	
	// Muzzle LineTrace
	FHitResult MuzzleLaserHit;
	// FCollisionQueryParams Params;
	// Params.AddIgnoredActor(this);
	// Params.AddIgnoredActor(GetOwner());
	
	FVector MuzzleStart = SocketTransform.GetLocation();		// 여기가 이상... 클라 1은 괜찮은데 서버는 이상..
	// CH_LOG(LogCHNetwork, Log, TEXT("%s"), *MuzzleStart.ToString())
	
	FVector MuzzleEnd;
	{
		if(HitLocation.Equals(FVector::ZeroVector))
		{
			MuzzleEnd = TraceEnd;
		}
		else
		{
			MuzzleEnd = MuzzleStart + (HitLocation - MuzzleStart) * 1.25f; 
		}
		UE_LOG(LogTemp, Log, TEXT("HitLocation.Equals(FVector::ZeroVector) : %d "), HitLocation.Equals(FVector::ZeroVector));
	
		// 총구에서 레이저
		GetWorld()->LineTraceSingleByChannel(MuzzleLaserHit, MuzzleStart, MuzzleEnd, ECollisionChannel::ECC_GameTraceChannel4);
		DrawDebugLine(GetWorld(), MuzzleStart, MuzzleEnd, FColor::Blue, false, 2);
		DrawDebugPoint(GetWorld(), MuzzleLaserHit.Location, 10, FColor::Blue, false, 2);
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

	AController* OwnerController = OwnerPawn->GetController();		
	if (OwnerController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerController"));
	}
	
	// 서버
	if(OwnerController)
	{
		if(HasAuthority())
		{
			const float DamageToCause = MuzzleLaserHit.BoneName.ToString() == FString("Head") ? HeadShotDamage : Damage;
			UE_LOG(LogTemp, Log, TEXT("MuzzleLaserHit : %s "), *GetNameSafe(MuzzleLaserHit.GetActor()))
			// UE_LOG(LogTemp, Log, TEXT("MuzzleLaserHit : %s , ScreenLaserHit : %s"), *GetNameSafe(MuzzleLaserHit.GetActor()),*GetNameSafe(MuzzleLaserHit.GetActor()));
			// if(MuzzleLaserHit.GetActor() == HitActor)		
			// {
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
	UE_LOG(LogTemp, Log, TEXT("MuzzleLaserHit : %s "), *GetNameSafe(MuzzleLaserHit.GetActor()))
	// 궤적
	{
		FVector BeamEnd = TraceEnd;
		if (MuzzleLaserHit.bBlockingHit)
		{
			// BeamEnd = Hit.ImpactPoint;
			BeamEnd = HitLocation;
		}
		else
		{
			MuzzleLaserHit.Location = TraceEnd;
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
				SocketTransform
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
		FHitResult Hit;
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
		bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel4, Params);
		UE_LOG(LogTemp, Log, TEXT("AttackTarget : %s , HitActor : %s"), *GetNameSafe(AttackTarget),*GetNameSafe(Hit.GetActor()));
	
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
		FHitResult Hit;
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
		bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel4, Params);
		UE_LOG(LogTemp, Log, TEXT("AttackTarget : %s , HitActor : %s"), *GetNameSafe(AttackTarget),*GetNameSafe(Hit.GetActor()));
	
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
	/*UE_LOG(LogTemp, Log, TEXT("[ACHGunRifle::StartAim()] Before bNearWall : %d"),OwningCharacter->GetNearWall());
	if(!OwningCharacter->GetCovered())
	{
		if(OwningCharacter->GetNearWall()) return;		
	}
	UE_LOG(LogTemp, Log, TEXT("[ACHGunRifle::StartAim()] After bNearWall : %d"),OwningCharacter->GetNearWall());*/
	if(bReloading)
	{
		// cancel aim
		StopAim();
		return;
	}
	bHoldGun = false;
	ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);

	AController* OwnerController = OwningCharacter->GetController();		
	if (OwnerController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerController"));
		return;
	}

	if(!HasAuthority())
	{
		// Viewport LineTrace	
		FVector TraceStart;
		FRotator Rotation;
		OwnerController->GetPlayerViewPoint(TraceStart, Rotation);
		DrawDebugCamera(GetWorld(), TraceStart, Rotation, 90, 2, FColor::Red, false,2);
		FVector TraceEnd = TraceStart + Rotation.Vector() * 1000.f;

		FVector Direction = TraceEnd -  HandleSocket_3P->GetSocketLocation(GetWeaponMesh3P());

		// 내분점	
		DrawDebugPoint(GetWorld(),TraceEnd,10.0f,FColor::Magenta,false,2);
		// 3인칭 메시 기준 
		DrawDebugLine(GetWorld(),HandleSocket_3P->GetSocketLocation(GetWeaponMesh3P()),TraceEnd,FColor::Magenta,false,2);
		// MuzzleCollision->SetRelativeLocation(HandleSocket_3P->GetSocketLocation(GetWeaponMesh3P()) + Direction.GetSafeNormal() * BarrelLength);
		MuzzleCollision->SetWorldLocation(HandleSocket_3P->GetSocketLocation(GetWeaponMesh3P()) + Direction.GetSafeNormal() * BarrelLength);
		FRotator MuzzleRotation = Direction.Rotation();
		MuzzleRotation.Pitch += 90.0f;
		MuzzleCollision->SetWorldRotation(MuzzleRotation);
		// Direction.GetSafeNormal()
	}
	
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
				PlayerCharacter->ReturnCover();
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
	// if(OwningCharacter->GetNearWall()) return;
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


