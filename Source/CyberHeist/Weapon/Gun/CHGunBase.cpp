// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/CHGunBase.h"

#include "CyberHeist.h"
#include "Camera/CameraComponent.h"
#include "Character/CHCharacterBase.h"
#include "Components/CapsuleComponent.h"

#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Character/CHCharacterPlayer.h"
#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMeshSocket.h"


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
	MuzzleCollision3P->SetupAttachment(WeaponMesh3P);	

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

	MuzzleCollision1P->OnComponentBeginOverlap.AddDynamic(this, &ACHGunBase::OnNearWall);
	MuzzleCollision1P->OnComponentEndOverlap.AddDynamic(this,&ACHGunBase::OnFarFromWall);
	
	MuzzleCollision3P->OnComponentBeginOverlap.AddDynamic(this, &ACHGunBase::OnNearWall);
	MuzzleCollision3P->OnComponentEndOverlap.AddDynamic(this,&ACHGunBase::OnFarFromWall);
}

void ACHGunBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 총구에서 레이저를 쏜다

	// if(OwningCharacter && !OwningCharacter->HasAuthority() && OwningCharacter->IsLocallyControlled())
	// 서버 말고 다른 사람에게만 보이기 
	
	
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
	if((OwningCharacter && !OwningCharacter->HasAuthority() && OwningCharacter->IsLocallyControlled()) || GetNetMode() == ENetMode::NM_Standalone)
	{		
		// 리플리로 총 내리기
		// 변수 하나 동기화해서 총 못쏘게 하기
		if(OtherActor == OwningCharacter || OtherActor == this)
		{
			return;
		}
		
		OwningCharacter->SetNearWall(true);

		// 총 내리기
		StopPrecisionAim();
		CancelPullTrigger();
		
		CH_LOG(LogCHNetwork, Log, TEXT("OtherActor : %s"), *OtherActor->GetName())
		CH_LOG(LogCHNetwork, Log, TEXT("OtherComp : %s"), *OtherComp->GetName())		// DetectWall?? 
					
		GetWorld()->GetTimerManager().ClearTimer(ShootTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
		
		CH_LOG(LogCHNetwork, Log, TEXT("[OnNearWall] %d"), OwningCharacter->GetNearWall());		
	}
}

void ACHGunBase::OnFarFromWall(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// ACHCharacterPlayer* CHPlayer = CastChecked<ACHCharacterPlayer>(OwningCharacter);
	
	/*if(CHPlayer->IsInFirstPersonPerspective())
	{
		// 1인칭 머즐..만 작동해야 하는데... 어캐 하지.. 이벤트를 풀었다가 할수는 없고... 
	}*/
	if(OwningCharacter && !OwningCharacter->HasAuthority() && OwningCharacter->IsLocallyControlled() || GetNetMode() == ENetMode::NM_Standalone)
	{	
		if(OtherActor == OwningCharacter || OtherActor == this)
		{
			return;
		}
		OwningCharacter->SetNearWall(false);
		StayPrecisionAim();
		
		CH_LOG(LogCHNetwork,Log, TEXT("[OnFarFromWall] %d"), OwningCharacter->GetNearWall());
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

	CH_LOG(LogCHTemp, Log, TEXT("Begin"))

	// NPC는 무시
	if(ACHCharacterPlayer* CHPlayer = Cast<ACHCharacterPlayer>(OwningCharacter))
	{
		if(CHPlayer->IsInFirstPersonPerspective())
		{
			MuzzleCollision1P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			MuzzleCollision3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			CH_LOG(LogCHTemp, Log, TEXT("1pp collision On"))
		}
		else
		{
			MuzzleCollision1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			MuzzleCollision3P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			CH_LOG(LogCHTemp, Log, TEXT("3pp collision On"))
		}
	}
	
	CH_LOG(LogCHTemp, Log, TEXT("End"))
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

	MuzzleCollision1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);	
	MuzzleCollision3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);	
	
	// WeaponMesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh1P->SetVisibility(false, true);
	WeaponMesh1P->CastShadow = false;
	
	// WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh3P->SetVisibility(false, true);
 	WeaponMesh3P->CastShadow = false;
	WeaponMesh3P->bCastHiddenShadow = false;
}

void ACHGunBase::DisableWeaponInput()
{
}

void ACHGunBase::Fire()
{
	
}

void ACHGunBase::LocalFire(const FVector& HitLocation, const FTransform& MuzzleTransform)
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
	ServerPullTrigger();
}

void ACHGunBase::CancelPullTrigger()
{
	ServerCancelPullTrigger();
}

void ACHGunBase::PlayFireVFX(const FTransform& HitTransform, const FTransform& MuzzleTransform)
{
}

void ACHGunBase::MulticastPlayFireVFX_Implementation(const FTransform& HitTransform, const FTransform& MuzzleTransform)
{
	if(HasAuthority()) return;
	PlayFireVFX(HitTransform, MuzzleTransform);
}

void ACHGunBase::ServerStartAim_Implementation()
{
	MulticastStartAim();
}

bool ACHGunBase::ServerStartAim_Validate()
{
	return true;
}

void ACHGunBase::MulticastStartAim_Implementation()
{
	OnStartAim();
}

void ACHGunBase::ServerStopAim_Implementation()
{
	MulticastStopAim();
}

bool ACHGunBase::ServerStopAim_Validate()
{
	return true;
}

void ACHGunBase::MulticastStopAim_Implementation()
{
	OnStopAim();
}

void ACHGunBase::ServerPullTrigger_Implementation()
{
	MulticastPullTrigger();
}

bool ACHGunBase::ServerPullTrigger_Validate()
{
	return true;
}

void ACHGunBase::MulticastPullTrigger_Implementation()
{
	OnPullTrigger();
}

void ACHGunBase::OnStartAim()
{
	if(OwningCharacter == nullptr) return;
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third)
		OwningCharacter->bUseControllerRotationYaw = true; 
}

void ACHGunBase::OnStopAim()
{
	if(OwningCharacter == nullptr) return;
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third)
		OwningCharacter->bUseControllerRotationYaw = false; 
}

void ACHGunBase::OnPullTrigger()
{
	if(OwningCharacter == nullptr) return;
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third)
		OwningCharacter->bUseControllerRotationYaw = true; 
}

void ACHGunBase::OnCancelPullTrigger()
{
	if(OwningCharacter == nullptr) return;
	if (OwningCharacter->CurrentCharacterControlType == ECharacterControlType::Third)
		OwningCharacter->bUseControllerRotationYaw = false; 
}

void ACHGunBase::ServerCancelPullTrigger_Implementation()
{
	MulticastCancelPullTrigger();
}

bool ACHGunBase::ServerCancelPullTrigger_Validate()
{
	return true;
}

void ACHGunBase::MulticastCancelPullTrigger_Implementation()
{
	OnCancelPullTrigger();
}

void ACHGunBase::SetWeaponMeshVisibility(uint8 bVisible)
{
}

void ACHGunBase::StartAim()
{
	if(OwningCharacter)
	{
		// 총의 IMC를 기존 플레이어 IMC 보다 높게 하기
		if(OwningCharacter->bNearWall)
		{
			// 총기 접은 몽타주 재생
			return;
		}
		ServerStartAim();		
	}	
}

void ACHGunBase::StopAim()
{
	ServerStopAim();
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
	ServerReload();
}

void ACHGunBase::LocalReload()
{
}

void ACHGunBase::MulticastReload_Implementation()
{
	LocalReload();
	// 몽타주
}

void ACHGunBase::ServerReload_Implementation()
{
	MulticastReload();  // 변수들은 문제 X, 몽타주는?
	// LocalReload();
}

bool ACHGunBase::ServerReload_Validate()
{
	return true;
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
	DOREPLIFETIME(ACHGunBase, OwningCharacter);	
}

void ACHGunBase::OnRep_Owner()
{
	Super::OnRep_Owner();
}

void ACHGunBase::MulticastRPCFire_Implementation(const FVector& HitLocation, const FTransform& MuzzleTransform)
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"));
	// OwningCharacter->bUseControllerRotationYaw = true;
	// 클라 && 서버 X => 클라 본인 제외 
	if(OwningCharacter->IsLocallyControlled() && !OwningCharacter->HasAuthority()) return;
	LocalFire(HitLocation, MuzzleTransform);		
}

void ACHGunBase::ServerRPCFire_Implementation(const FVector& HitLocation, const FTransform& MuzzleTransform)
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"));
	// Muzzle에서 ray쏘기
	// 데미지
	
	MulticastRPCFire(HitLocation, MuzzleTransform);		// 
}

bool ACHGunBase::ServerRPCFire_Validate(const FVector& HitLocation, const FTransform& MuzzleTransform)
{
	return true;
}