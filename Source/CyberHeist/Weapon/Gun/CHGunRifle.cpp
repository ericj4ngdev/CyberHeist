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
}

void ACHGunRifle::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		// SetOwner();
		AActor* OwnerActor = GetOwner();
		if(OwnerActor)
		{
			CH_LOG(LogCHNetwork, Log, TEXT("Owner : %s"), *OwnerActor->GetName())

			CH_LOG(LogCHNetwork, Log, TEXT("Owner : %s"), GetLocalRole())
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

	// UE_LOG(LogTemp,Warning,TEXT("[ACHGunRifle::Tick] ScopeCamLoc: [%s]"), *ScopeCamera->GetComponentLocation().ToString())
	/*if(OwningCharacter)
	{
		APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController());
		UE_LOG(LogTemp,Warning,TEXT("[ACHGunRifle::Tick] PlayerController->GetControlRotation(): [%s]"), *PlayerController->GetControlRotation().ToString())
		
	}*/
}
void ACHGunRifle::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ACHCharacterBase* CharacterBase = Cast<ACHCharacterBase>(OtherActor);
	CharacterBase->ClientRPCAddIMC(this,FireMappingContext);
}

/*void ACHGunRifle::NotifyActorBeginOverlap(AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);
	if(HasAuthority())
	{
		ACHCharacterBase* CharacterBase = Cast<ACHCharacterBase>(Other);
		CharacterBase->ClientRPCAddIMC(this,FireMappingContext);
	}
}*/

// 캐릭터를 받아올까? 뭐가 문제일까?
void ACHGunRifle::Equip()
{
	Super::Equip();

	if(!bIsEquipped) return;
	// OwningCharacter->SetHasRifle(true);

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

	// 오버랩 이벤트를 클라가 받으면 클라에서만 일어나야 함.
	// 오버랩 끝났을 때? 
	// 클라 RPC
	// 액터컴포넌트를 추가시키면 자동 장착
	// Set up action bindings
	/*if (APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
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
	}*/
	// 캐릭터 RPC. 총의 소유권이 서버에게 있어서 캐릭터에서 RPC를 받아야 한다.
	// 인자로 IMC를 받아서 추가하는 함수를 캐릭터에서 호출한다. 
}

void ACHGunRifle::UnEquip()
{
	Super::UnEquip();

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

void ACHGunRifle::Fire()
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
	
	FTransform SocketTransform;
	if(OwningCharacter->IsInFirstPersonPerspective())
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

	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);
	// DrawDebugCamera(GetWorld(), Location, Rotation, 90, 2, FColor::Red, true);
	
	// LineTrace
	FHitResult ScreenLaserHit;
	FHitResult MuzzleLaserHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	FVector TraceEnd = Location + Rotation.Vector() * MaxRange;
	
	// 화면 중앙 레이저
	bool bScreenLaserSuccess = GetWorld()->LineTraceSingleByChannel(ScreenLaserHit, Location, TraceEnd, ECollisionChannel::ECC_GameTraceChannel4, Params);
	DrawDebugLine(GetWorld(),Location, TraceEnd,FColor::Red,false, 2);
	DrawDebugPoint(GetWorld(), ScreenLaserHit.Location, 10, FColor::Red, false, 2);

	FVector HitLocation = ScreenLaserHit.Location;
	AActor* HitActor = ScreenLaserHit.GetActor();
	UE_LOG(LogTemp, Log, TEXT("HitLocation : %s "), *HitLocation.ToString());
	FVector MuzzleStart = SocketTransform.GetLocation();

	FVector MuzzleEnd;
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
	
	const float DamageToCause = ScreenLaserHit.BoneName.ToString() == FString("Head") ? HeadShotDamage : Damage;
	UE_LOG(LogTemp, Log, TEXT("MuzzleLaserHit : %s , ScreenLaserHit : %s"), *GetNameSafe(MuzzleLaserHit.GetActor()),*GetNameSafe(ScreenLaserHit.GetActor()));

	// 위치가 같다면 정중앙 효과
	// 다르면 총구 레이저 효과
	
	if(MuzzleLaserHit.Location.Equals(HitLocation))
	{
		if(ImpactEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation
			(
				GetWorld(),
				ImpactEffect,
				ScreenLaserHit.Location, 
				ScreenLaserHit.ImpactNormal.Rotation()
			);			
		}
	}
	else
	{
		if(ImpactEffect)
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
	
	if(MuzzleLaserHit.GetActor() == HitActor)		
	{		
		ACHCharacterBase* CharacterBase = Cast<ACHCharacterBase>(ScreenLaserHit.GetActor());
		if (CharacterBase)
		{
			FPointDamageEvent DamageEvent(DamageToCause, ScreenLaserHit, ScreenLaserHit.ImpactNormal, nullptr);
			CharacterBase->TakeDamage(DamageToCause, DamageEvent, OwnerController, this);
		}
		else
		{
			UGameplayStatics::ApplyDamage(HitActor,DamageToCause,OwnerController,this,UDamageType::StaticClass());
			UE_LOG(LogTemp, Log, TEXT("HitActor : %s"), *GetNameSafe(HitActor))
		}
	}	
	
	// 궤적
	FVector BeamEnd = TraceEnd;
	if (ScreenLaserHit.bBlockingHit)
	{
		// BeamEnd = Hit.ImpactPoint;
		BeamEnd = ScreenLaserHit.Location;
	}
	else
	{
		ScreenLaserHit.Location = TraceEnd;
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
			ScreenLaserHit.ImpactPoint
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
		// OwningCharacter->SetAiming(true);

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
	// AttackTargetActor = nullptr;
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
	// UE_LOG(LogTemp, Log, TEXT("[ACHGunRifle::StartAim()] Before bNearWall : %d"),OwningCharacter->GetNearWall());
	/*if(!OwningCharacter->GetCovered())
	{
		if(OwningCharacter->GetNearWall()) return;		
	}*/
	// UE_LOG(LogTemp, Log, TEXT("[ACHGunRifle::StartAim()] After bNearWall : %d"),OwningCharacter->GetNearWall());
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
					PlayerController->SetViewTargetWithBlend(this,0.2f);
					OwningCharacter->GetFirstPersonMesh()->SetVisibility(false);	// 팔 보이게 하기
				}
			}
			else
			{			
				PlayerCharacter->SetCharacterControl(ECharacterControlType::FirstAim);	
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
		}
	
		if(PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::FirstScopeAim)
		{
			PlayerCharacter->SetCharacterControl(ECharacterControlType::First);
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
			PlayerCharacter->SetCharacterControl(ECharacterControlType::ThirdPrecisionAim);
		}

		if(PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::FirstAim)
		{
			PlayerCharacter->SetScopeAiming(true);
			PlayerCharacter->SetCharacterControl(ECharacterControlType::FirstScopeAim);
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
			PlayerCharacter->SetCharacterControl(ECharacterControlType::ThirdAim);		
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
			PlayerCharacter->SetCharacterControl(ECharacterControlType::FirstAim);
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

void ACHGunRifle::FirstLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	// UE_LOG(LogTemp, Warning, TEXT("LookAxisVector.X : %f, LookAxisVector.Y : %f"), LookAxisVector.X, LookAxisVector.Y);
	if(OwningCharacter)
	{
		// float Pitch = FMath::Clamp(LookAxisVector.Y, 80,-80);
		ACHCharacterPlayer* PlayerCharacter = Cast<ACHCharacterPlayer>(OwningCharacter);
		if(PlayerCharacter->CurrentCharacterControlType == ECharacterControlType::FirstScopeAim)
		{
			float Pitch = LookAxisVector.Y;
			UE_LOG(LogTemp, Warning, TEXT("LookAxisVector.X : %f, LookAxisVector.Y : %f"), LookAxisVector.X, LookAxisVector.Y);
			// ScopeCamera->SetRelativeRotation(FRotator(Pitch,LookAxisVector.X,Pitch));

			// 총기 자체를 움직이게 하자. 확대 조준시 손은 없애자.		
			// AddActorLocalRotation(FRotator(0,0,Pitch));
			WeaponMesh1P->AddRelativeRotation(FRotator(0,0,Pitch));
			// 걍 여기서 캐릭터의 Rotation을 주자.
			// 하지만 아래와 같이 해도 캐릭터는 움직이지 않는다. 
			// OwningCharacter->AddControllerPitchInput(LookAxisVector.Y);
			// OwningCharacter->bUseControllerRotationPitch = true;
		
			// ScopeCamera->AddRelativeRotation(FRotator(-Pitch,0,0));
			UE_LOG(LogTemp, Warning, TEXT("Pitch : %f"), Pitch);
		
		
			// OwningCharacter->AddControllerYawInput(LookAxisVector.X);
			// OwningCharacter->AddControllerPitchInput(LookAxisVector.Y);		// modify
		}
		else
		{
			// SetActorRotation(FRotator(0, 0, -90.0f));
			WeaponMesh1P->SetRelativeRotation(FRotator(0, 0, -90.0f));
		}
	}
	// UE_LOG(LogTemp, Warning, TEXT("LookAxisVector.X : %f, LookAxisVector.Y : %f"), LookAxisVector.X, LookAxisVector.Y);	
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