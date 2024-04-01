// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "InputActionValue.h"
#include "GameFramework/Actor.h"
#include "CHGun.generated.h"

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

	UPROPERTY(VisibleAnywhere, Category = "Effect")
	TObjectPtr<class UParticleSystem> MuzzleFlash;  // UParticleSystem

	UPROPERTY(VisibleAnywhere, Category = "Effect")
	TObjectPtr<class UParticleSystem> ImpactEffect;  // UParticleSystem
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FVector WeaponMeshPickupRelativeLocation;

	TObjectPtr<class ACHCharacterPlayer> OwningCharacter;

public:
	// Pickup on touch
	virtual void NotifyActorBeginOverlap(class AActor* Other) override;
	
	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void AttachWeapon(ACHCharacterPlayer* TargetCharacter);

	// Whether or not to spawn this weapon with collision enabled (pickup mode).
	// Set to false when spawning directly into a player's inventory or true when spawning into the world in pickup mode.
	UPROPERTY(BlueprintReadWrite)
	bool bSpawnWithCollision;
	
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

public:	
	void PullTrigger();
	void CancelPullTrigger();
	void StartAim();
	void StopAim();
	void StopParticleSystem();
	void FireLine();
	// properties
public:
	uint8 bTrigger : 1;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float FireInterval = 0.1f;
	FTimerHandle DurationTimerHandle;
	FTimerHandle FireTimerHandle;
	FTimerHandle ShootTimerHandle;
	float ShootingPreparationTime = 0.2f;
private:
	UPROPERTY(EditAnywhere)
	float MaxRange = 5000;
	
	UPROPERTY(EditAnywhere)
		float Damage = 10;

	
public:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ACHProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TObjectPtr <class UAnimMontage> FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;


};
