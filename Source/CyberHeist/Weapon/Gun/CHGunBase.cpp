// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/CHGunBase.h"

#include "CyberHeist.h"
#include "Camera/CameraComponent.h"
#include "Character/CHCharacterBase.h"
#include "Components/CapsuleComponent.h"

#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACHGunBase::ACHGunBase()
{ 	
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root"));
	CollisionComp->InitCapsuleSize(40.0f, 50.0f);
	CollisionComp->SetCollisionObjectType(ECC_GameTraceChannel1);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // Manually enable when in pickup mode
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = CollisionComp;

	WeaponMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(FName("WeaponMesh1P"));
	WeaponMesh1P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponMesh1P->CastShadow = false;
	WeaponMesh1P->SetVisibility(false, false);	
	WeaponMesh1P->SetupAttachment(CollisionComp);
	// WeaponMesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	
	WeaponMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh3P"));
	WeaponMesh3P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponMesh3P->SetupAttachment(CollisionComp);	
	WeaponMesh3P->CastShadow = true;
	
	WeaponMesh3P->SetVisibility(true, false);
	WeaponMesh3P->SetupAttachment(CollisionComp);
	// WeaponMesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;

	ScopeCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ScopeCamera"));
	ScopeCamera->SetupAttachment(WeaponMesh1P);	
	ScopeCamera->bUsePawnControlRotation = true;	

	MuzzleCollision3P = CreateDefaultSubobject<UCapsuleComponent>(TEXT("MuzzleCollision3P"));
	// MuzzleCollision->SetRelativeLocation()
	MuzzleCollision3P->SetupAttachment(WeaponMesh3P);	
	// MuzzleCollision->InitCapsuleSize(40.0f, 50.0f);

	MuzzleCollision1P= CreateDefaultSubobject<UCapsuleComponent>(TEXT("MuzzleCollision1P"));
	MuzzleCollision1P->SetupAttachment(WeaponMesh1P);
	
	bReloading = false;	
	bHoldGun = true;
}

// Called when the game starts or when spawned
void ACHGunBase::BeginPlay()
{
	Super::BeginPlay();
	// Effect->AttachToComponent(WeaponMesh3P, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("MuzzleFlashSocket"));
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ACHGunBase::OnSphereBeginOverlap);

	MuzzleCollision3P->OnComponentBeginOverlap.AddDynamic(this, &ACHGunBase::OnNearWall);
	MuzzleCollision3P->OnComponentEndOverlap.AddDynamic(this,&ACHGunBase::OnFarFromWall);
}

void ACHGunBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 총구에서 레이저를 쏜다
	// 총구 앞에 Hit가 되면 몽타주 재생
	
	/*FTransform SocketTransform;
	if(OwningCharacter->IsInFirstPersonPerspective())
	{
		const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh1P()->GetSocketByName();
		SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh1P());
		if(MuzzleFlashSocket == nullptr) return; 
	}
	else
	{
		const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh3P()->GetSocketByName("MuzzleFlash");
		SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh3P());
		if(MuzzleFlashSocket == nullptr) return; 
	}*/
	/*if(!HasAuthority())
	{*/
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
			FCollisionQueryParams TraceParams(FName(TEXT("Coveace")), true, this);
			// Params.AddIgnoredActor(this);
			// TraceParams.AddIgnoredActor(GetOwner());
			// bool HitDetected = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel1, Params);
			bool HitDetected = GetWorld()->SweepSingleByChannel(HitResult, Start, End,FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(CapsuleRadius), TraceParams);
		
			FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
			// Debug 캡슐 그리기
			DrawDebugCapsule(GetWorld(), CapsuleLocation, CapsuleHalfHeight, CapsuleRadius, CapsuleRotation.Quaternion(), DrawColor);
		}
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
		FCollisionQueryParams TraceParams(FName(TEXT("Coveace")), true, this);
		// Params.AddIgnoredActor(this);
		// TraceParams.AddIgnoredActor(GetOwner());
		// bool HitDetected = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel1, Params);
		bool HitDetected = GetWorld()->SweepSingleByChannel(HitResult, Start, End,FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(CapsuleRadius), TraceParams);
		
		FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
		// Debug 캡슐 그리기
		DrawDebugCapsule(GetWorld(), CapsuleLocation, CapsuleHalfHeight, CapsuleRadius, CapsuleRotation.Quaternion(), DrawColor);
	}
}

void ACHGunBase::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(HasAuthority())
	{
     	ACHCharacterBase* CharacterBase = Cast<ACHCharacterBase>(OtherActor);
     	if(CharacterBase)
     	{
     		bIsEquipped = true;
     		CharacterBase->AddWeaponToInventory(this,bIsEquipped);
     	}
	}
}
     
UAnimMontage* ACHGunBase::GetEquip1PMontage() const
{
	return Equip1PMontage;
}

UAnimMontage* ACHGunBase::GetEquip3PMontage() const
{
	return Equip3PMontage;
}

void ACHGunBase::OnNearWall(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(HasAuthority())
	{
		// 리플리로 총 내리기
		// 변수 하나 동기화해서 총 못쏘게 하기
		if(OwningCharacter)
		{
			if(OtherActor == OwningCharacter)
			{
				return;
			}
			
			OwningCharacter->SetNearWall(true);

			// 총 내리기
			StopPrecisionAim();
			CancelPullTrigger();

			UE_LOG(LogTemp, Warning, TEXT("OtherActor : %s"), *OtherActor->GetName());
			UE_LOG(LogTemp, Warning, TEXT("OtherComp : %s"), *OtherComp->GetName());
						
			GetWorld()->GetTimerManager().ClearTimer(ShootTimerHandle);
			GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
			
			CH_LOG(LogCHNetwork, Log, TEXT("[OnNearWall] %d"), OwningCharacter->GetNearWall());
		}
	}
}

void ACHGunBase::OnFarFromWall(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(HasAuthority())
	{
		if(OwningCharacter)
		{
			if(OtherActor == OwningCharacter)
			{
				return;
			}
			OwningCharacter->SetNearWall(false);
			StayPrecisionAim();
		
			CH_LOG(LogCHNetwork,Log, TEXT("[OnFarFromWall] %d"), OwningCharacter->GetNearWall());
		}
	}
	
	// 기존에 확대 조준 중이었으면 다시 확대 조준으로 돌아가기
	
}

void ACHGunBase::Equip()
{
	if (OwningCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s %s OwningCharacter is nullptr"), *FString(__FUNCTION__), *GetName());
		return;
	}
	
	bIsEquipped = true;
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACHGunBase::UnEquip()
{
	if (OwningCharacter == nullptr)
	{
		return;
	}

	bIsEquipped = false;

	// WeaponMesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh1P->SetVisibility(false, true);
	WeaponMesh1P->CastShadow = false;
	
	// WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh3P->SetVisibility(false, true);
 	WeaponMesh3P->CastShadow = false;
	WeaponMesh3P->bCastHiddenShadow = false;
}

void ACHGunBase::Fire()
{
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

	if(!OwningCharacter->HasAuthority())
	{
		LocalFire(HitLocation, TraceEnd);
	}
	ServerRPCFire(HitLocation, TraceEnd);	
}

void ACHGunBase::LocalFire(const FVector& HitLocation,const FVector& TraceEnd)
{
	// 쏘는 몽타주가 여기 있다. 총알이 다 차거나 재장전 중일 때 예외처리는 여기서 해야할 듯. 
	if(!bIsEquipped) return;
	if(bReloading || CurrentAmmoInClip <= 0) return;
}

void ACHGunBase::PullTriggerByAI(AActor* AttackTarget)
{
	
}

void ACHGunBase::FireByAI(AActor* AttackTarget)
{

}

void ACHGunBase::AutoFireByAI(AActor* AttackTarget)
{
}

void ACHGunBase::EndShoot()
{
	if(!bIsEquipped) return;
	
	// if(OwningCharacter)	OwningCharacter->SetCombatMode(false);
	GetWorld()->GetTimerManager().ClearTimer(ShootTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);		// 이래야 EndShoot 반복 가능
	// 다끝나고 초기화
	bTrigger = false;
	
	OwningCharacter->NotifyComboActionEnd();	 // AttackTask return Succeeded
}

void ACHGunBase::PullTrigger()
{
	
}

void ACHGunBase::CancelPullTrigger()
{
	
}

void ACHGunBase::SetWeaponMeshVisibility(uint8 bVisible)
{
}

void ACHGunBase::StartAim()
{
	// 총의 IMC를 기존 플레이어 IMC 보다 높게 하기
	if(OwningCharacter->bNearWall)
	{
		// 총기 접은 몽타주 재생
		return;
	}
}

void ACHGunBase::StopAim()
{
	// 총의 IMC를 기존 플레이어 IMC 보다 낮게 하기 
}

void ACHGunBase::StartPrecisionAim()
{
	
}

void ACHGunBase::StopPrecisionAim()
{
	
}

void ACHGunBase::StayPrecisionAim()
{
}

void ACHGunBase::Reload()
{
}

void ACHGunBase::SetupWeaponInputComponent()
{
	// SetOwningCharacter();
}

void ACHGunBase::SetOwningCharacter(ACHCharacterBase* InOwningCharacter)
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"),TEXT("Begin"))
	AActor* OwnerActor = GetOwner();
	if(OwnerActor)
	{
		CH_LOG(LogCHNetwork, Log, TEXT("Owner : %s"), *OwnerActor->GetName())
	}
	else
	{
		CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("No Owner"))
	}
	
	OwningCharacter = InOwningCharacter;
	OwnerActor = OwningCharacter;
	if (OwningCharacter == nullptr)	return;
	SetOwner(OwningCharacter);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if(OwnerActor)
	{
		CH_LOG(LogCHNetwork, Log, TEXT("Owner : %s"), *OwnerActor->GetName())
	}
	else
	{
		CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("No Owner"))
	}
	// CH_LOG(LogCHNetwork, Log, TEXT("GetNetOwningPlayer : %s"),*GetNetOwningPlayer()->GetName())
	CH_LOG(LogCHNetwork, Log, TEXT("%s"),TEXT("End"))
	
	/*if (OwningCharacter->GetCurrentWeapon() != this)
	{
		WeaponMesh3P->CastShadow = false;
		WeaponMesh3P->SetVisibility(true, true);
		WeaponMesh3P->SetVisibility(false, true);
	}*/
}

void ACHGunBase::StopParticleSystem()
{
	// Effect->Deactivate();
	// UE_LOG(LogTemp, Warning, TEXT("StopParticleSystem"));
}

void ACHGunBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACHGunBase, bIsEquipped);
	DOREPLIFETIME(ACHGunBase, bTrigger);
	DOREPLIFETIME(ACHGunBase, bReloading);
	DOREPLIFETIME(ACHGunBase, bHoldGun);	
}

void ACHGunBase::OnRep_Owner()
{
	Super::OnRep_Owner();
}

void ACHGunBase::MulticastRPCFire_Implementation(const FVector& HitLocation, const FVector& TraceEnd)
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"));

	// 클라 && 서버 X => 클라 본인 제외 
	if(OwningCharacter->IsLocallyControlled() && !OwningCharacter->HasAuthority()) return;
	LocalFire(HitLocation, TraceEnd);
}

void ACHGunBase::ServerRPCFire_Implementation(const FVector& HitLocation, const FVector& TraceEnd)
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"));
	MulticastRPCFire(HitLocation, TraceEnd);
}

bool ACHGunBase::ServerRPCFire_Validate(const FVector& HitLocation, const FVector& TraceEnd)
{
	return true;
}