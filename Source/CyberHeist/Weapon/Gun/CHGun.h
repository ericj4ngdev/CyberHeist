// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "CHGun.generated.h"

UENUM(BlueprintType)
enum class EWeaponShootType : uint8
{
	LineTrace UMETA(DisplayName = "Line Trace"),
	Projectile UMETA(DisplayName = "Projectile")
};

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	Automatic UMETA(DisplayName = "Automatic"),
	SemiAutomatic UMETA(DisplayName = "Semi Automatic"),
	BurstFire UMETA(DisplayName = "BurstFire")
};


class ACHCharacterPlayer;

UCLASS()
class CYBERHEIST_API ACHGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACHGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category = Property)
	FString GunName;	

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimInstance> WeaponAnimInstance;*/
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UCapsuleComponent> CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> WeaponMesh1P;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> WeaponMesh3P;

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
	TObjectPtr<class UAnimMontage> ReloadWeaponMontage;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Animation")
	UAnimMontage* GetEquip1PMontage() const;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	UAnimMontage* GetEquip3PMontage() const;


	
	// Relative Location of weapon 1P Mesh when equipped
	UPROPERTY(EditAnywhere, Category = "Weapon")
	FVector WeaponMesh1PEquippedRelativeLocation;

	// Relative Location of weapon 3P Mesh when equipped
	UPROPERTY(EditAnywhere, Category = "Weapon")
	FVector WeaponMesh3PEquippedRelativeLocation;
	
	// Relative Location of weapon 1P Mesh when equipped
	UPROPERTY(EditAnywhere, Category = "Weapon|Location")
	FVector WeaponMeshFPEquippedRelativeLocation;

	// Relative Location of weapon 3P Mesh when equipped
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Location")
	FVector WeaponMeshTPEquippedRelativeLocation;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UParticleSystemComponent> Effect;

	UPROPERTY(EditAnywhere, Category = "Weapon|Effect")
	TObjectPtr<class UParticleSystem> MuzzleFlash;  // UParticleSystem

	UPROPERTY(EditAnywhere, Category = "Weapon|Effect")
	TObjectPtr<class UParticleSystem> ImpactEffect;  // UParticleSystem

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Audio")
	TObjectPtr<class USoundBase> FireSound;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Muzzle")
	FVector MuzzleOffset;

	// properties
public:
	/** Projectile class to spawn */
	UPROPERTY(EditAnywhere, Category = "Weapon|Properties")
	TSubclassOf<class ACHProjectile> ProjectileClass;

	// How much ammo in the clip the gun starts with
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ammo")
	int32 CurrentAmmoInClip;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ammo")
	int32 ClipSize;

	// Ammo except in clip
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ammo")
	int32 CurrentAmmo;
	
	// maximum bullet capacity
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ammo")
	int32 MaxAmmoCapacity;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ammo")
	uint8 bReloading : 1;	
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ammo")
	uint8 bInfiniteAmmo : 1;
	
	UPROPERTY(EditAnywhere, Category = "Weapon|Properties")
	float FireInterval = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Weapon|Properties")
	float ReloadInterval = 2.f;
	
	UPROPERTY(EditAnywhere, Category = "Weapon|Properties")
	float ShootingPreparationTime = 0.2f;
	
	UPROPERTY(EditAnywhere, Category = "Weapon|Properties")
	float MaxRange;
	
	UPROPERTY(EditAnywhere, Category = "Weapon|Properties")
	float Damage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Properties")
	EWeaponShootType ShootingType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Properties")
	EFireMode FireMode;

	EFireMode DefaultFireMode;
	EWeaponShootType DefaultShootingType;
	
	FTimerHandle DurationTimerHandle;	
	FTimerHandle FireTimerHandle;
	FTimerHandle ShootTimerHandle;
	FTimerHandle ReloadTimerHandle;

	uint8 bTrigger : 1;

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
	
	void PullTriggerProjectile();
	void PullTriggerLine();
	void CancelPullTrigger();
	void StartAim();
	void StopAim();
	void StartPrecisionAim();
	void StopPrecisionAim();
	void StopParticleSystem();
	void Reload();

	/** Make the weapon Fire a Line */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void FireLine();

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void FireProjectile();

	/** Make the weapon Fire */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	uint8 FireByAI();
	void EndShoot();

private:
	TObjectPtr<class ACHCharacterBase> OwningCharacter;
	
public:
	// Pickup on touch
	virtual void NotifyActorBeginOverlap(class AActor* Other) override;
	
	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Equip();
	void UnEquip();
	void SetOwningCharacter(ACHCharacterBase* InOwningCharacter);

	bool bIsEquipped;
	// Called when the player picks up this weapon
	virtual void PickUpOnTouch(ACHCharacterBase* InCharacter);

	
};
