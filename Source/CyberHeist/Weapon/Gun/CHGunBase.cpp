// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/CHGunBase.h"
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

// Sets default values
ACHGunBase::ACHGunBase()
{ 	
	PrimaryActorTick.bCanEverTick = false;

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
	WeaponMesh1P->SetVisibility(false, true);
	WeaponMesh1P->SetupAttachment(CollisionComp);
	WeaponMesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	
	WeaponMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh3P"));
	WeaponMesh3P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponMesh3P->SetupAttachment(CollisionComp);
	// WeaponMesh3P->SetRelativeLocation(WeaponMesh3PickupRelativeLocation);
	WeaponMesh3P->CastShadow = true;
	WeaponMesh3P->SetVisibility(true, true);
	WeaponMesh3P->SetupAttachment(CollisionComp);
	WeaponMesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;

	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));
	Effect->bAutoActivate = false;
	Effect->SetupAttachment(CollisionComp);

	bReloading = false;
	bInfiniteAmmo = false;
}

// Called when the game starts or when spawned
void ACHGunBase::BeginPlay()
{
	Super::BeginPlay();
	Effect->AttachToComponent(WeaponMesh3P, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("MuzzleFlashSocket"));
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);	
}

void ACHGunBase::NotifyActorBeginOverlap(AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);

	PickUpOnTouch(Cast<ACHCharacterBase>(Other));	
}

UAnimMontage* ACHGunBase::GetEquip1PMontage() const
{
	return Equip1PMontage;
}

UAnimMontage* ACHGunBase::GetEquip3PMontage() const
{
	return Equip3PMontage;
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
	WeaponMesh3P->SetVisibility(false, false);
	WeaponMesh3P->CastShadow = false;
	
	if (APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

void ACHGunBase::Fire()
{
	
}

uint8 ACHGunBase::FireByAI()
{
	if(OwningCharacter)
	{
		OwningCharacter->SetAiming(true);
		Fire();
		GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ACHGunBase::EndShoot, FireInterval, false);	// End Attack.
		return true;
	}
	return false;
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

void ACHGunBase::StartAim()
{
	// 총의 IMC를 기존 플레이어 IMC 보다 높게 하기
	
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

void ACHGunBase::Reload()
{
}

void ACHGunBase::SetOwningCharacter(ACHCharacterBase* InOwningCharacter)
{
	OwningCharacter = InOwningCharacter;
	if (OwningCharacter == nullptr)	return;
	SetOwner(OwningCharacter);
	AttachToComponent(OwningCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/*if (OwningCharacter->GetCurrentWeapon() != this)
	{
		WeaponMesh3P->CastShadow = false;
		WeaponMesh3P->SetVisibility(true, true);
		WeaponMesh3P->SetVisibility(false, true);
	}*/
}

void ACHGunBase::PickUpOnTouch(ACHCharacterBase* InCharacter)
{
	bIsEquipped = true;
	InCharacter->AddWeaponToInventory(this,true);
}

void ACHGunBase::StopParticleSystem()
{
	Effect->Deactivate();
	// UE_LOG(LogTemp, Warning, TEXT("StopParticleSystem"));
}

