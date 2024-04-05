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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UCapsuleComponent> CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> WeaponMesh;

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
	
	UPROPERTY(EditAnywhere, Category = "Weapon|Properties")
	float FireInterval = 0.1f;
	
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

	void PullTriggerProjectile();
	void PullTriggerLine();
	void CancelPullTrigger();
	void StartAim();
	void StopAim();
	void StopParticleSystem();

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
