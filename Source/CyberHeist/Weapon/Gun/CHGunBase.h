// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "CHGunBase.generated.h"


UENUM(BlueprintType)
enum class ECHFireMode : uint8
{
	None			UMETA(DisplayName = "None"),
	FullAuto		UMETA(DisplayName = "FullAuto"),
	SemiAuto		UMETA(DisplayName = "SemiAuto"),
	BurstFire		UMETA(DisplayName = "BurstFire")
};

UENUM(BlueprintType)
enum class ECHWeaponType : uint8
{
	UnArmed,
	Rifle,
	MiniGun,
	RPG,
};


class ACHCharacterBase;

UCLASS()
class CYBERHEIST_API ACHGunBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACHGunBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Pickup on touch
	virtual void NotifyActorBeginOverlap(class AActor* Other) override;

protected:
	TObjectPtr<class ACHCharacterBase> OwningCharacter;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UCapsuleComponent> CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> WeaponMesh1P;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> WeaponMesh3P;

public:
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh1P() const {return WeaponMesh1P;}
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh3P() const {return WeaponMesh3P;}
	
	// Montage
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> Equip1PMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> Equip3PMontage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> Fire1PMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> ScopeFire1PMontage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> Fire3PMontage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> AimFire1PMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> Reload1PMontage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> Reload3PMontage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> WeaponMeshFireMontage;
	
	/* 총 자체 재장전 애니메이션*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> ReloadWeaponMontage;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Animation")
	UAnimMontage* GetEquip1PMontage() const;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	UAnimMontage* GetEquip3PMontage() const;

	// Offset
protected:
	// Relative Location of weapon 1P Mesh when equipped
	UPROPERTY(EditAnywhere, Category = "CHGunBase")
	FVector WeaponMesh1PEquippedRelativeLocation;

	// Relative Location of weapon 3P Mesh when equipped
	UPROPERTY(EditAnywhere, Category = "CHGunBase")
	FVector WeaponMesh3PEquippedRelativeLocation;
	
	UPROPERTY(VisibleAnywhere, Category = "CHGunBase|Effect")
	TObjectPtr<class UParticleSystemComponent> Effect;

	UPROPERTY(EditAnywhere, Category = "CHGunBase|Effect")
	TObjectPtr<class UParticleSystem> MuzzleFlash;

	UPROPERTY(EditAnywhere, Category = "CHGunBase|Effect")
	TObjectPtr<class UParticleSystem> ImpactEffect;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CHGunBase|Effect")
	FVector MuzzleOffset;
	
	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CHGunBase|Audio")
	TObjectPtr<class USoundBase> FireSound;

	// properties
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CHGunBase|Properties")
	ECHWeaponType WeaponType;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CHGunBase|Properties")
	ECHFireMode DefaultFireMode;
	
	UPROPERTY(EditAnywhere, Category = "CHGunBase|Properties")
	float FireInterval = 0.1f;

	UPROPERTY(EditAnywhere, Category = "CHGunBase|Properties")
	float ReloadInterval = 2.f;
	
	UPROPERTY(EditAnywhere, Category = "CHGunBase|Properties")
	float ShootingPreparationTime = 0.2f;
	
	UPROPERTY(EditAnywhere, Category = "CHGunBase|Properties")
	float MaxRange;
	
	UPROPERTY(EditAnywhere, Category = "CHGunBase|Properties")
	float Damage;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CHGunBase|Properties")
	ECHFireMode FireMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CHGunBase|Properties")
	FName AttachPoint1P;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CHGunBase|Properties")
	FName AttachPoint3P;

	// How much ammo in the clip the gun starts with
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CHGunBase|Ammo")
	int32 CurrentAmmoInClip;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CHGunBase|Ammo")
	int32 ClipSize;

	// Ammo except in clip
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CHGunBase|Ammo")
	int32 CurrentAmmo;
	
	// maximum bullet capacity
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CHGunBase|Ammo")
	int32 MaxAmmoCapacity;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CHGunBase|Ammo")
	uint8 bInfiniteAmmo : 1;

	
	
	FTimerHandle DurationTimerHandle;	
	FTimerHandle FireTimerHandle;
	FTimerHandle ShootTimerHandle;
	FTimerHandle ReloadTimerHandle;

	// Input
public:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FireAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AimAction;

	/** Precision Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> PrecisionAimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CancelPrecisionAimAction;

	/** Reload Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ReloadAction;

public:
	
	virtual void Equip();
	virtual void UnEquip();
	virtual void Fire();
	virtual uint8 FireByAI();
	virtual void EndShoot();
	virtual void PullTrigger();
	virtual void CancelPullTrigger();
	virtual void StartAim();
	virtual void StopAim();
	virtual void StartPrecisionAim();
	virtual void StopPrecisionAim();
	virtual void Reload();

	virtual void SetOwningCharacter(ACHCharacterBase* InOwningCharacter);	
	virtual void PickUpOnTouch(ACHCharacterBase* InCharacter);	
	virtual void StopParticleSystem();

	// state
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CHGunBase|State")
	uint8 bIsEquipped : 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CHGunBase|State")
	uint8 bTrigger : 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CHGunBase|State")
	uint8 bReloading : 1;	
	// virtual void SetWeaponProperties();
};
