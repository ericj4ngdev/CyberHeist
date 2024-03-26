// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "InputActionValue.h"
#include "CHWeaponComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnCombatModeSignature);
class ACHCharacterPlayer;

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CYBERHEIST_API UCHWeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	FOnCombatModeSignature OnCombat;

public:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ACHProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TObjectPtr < class UAnimMontage> FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TObjectPtr<class UParticleSystem> MuzzleFlash;  // UParticleSystem

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AimAction;

	/** Sets default values for this component's properties */
	UCHWeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void AttachWeapon(ACHCharacterPlayer* TargetCharacter);


	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Fire();

protected:
	void PullTrigger();
	void CancelPullTrigger();
	void StartAim();
	void StopAim();

	void StopParticleSystem();
	FTimerHandle DurationTimerHandle;

	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// 발사 간격(초)
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float FireInterval = 0.1f;

	// 발사 타이머 핸들
	FTimerHandle FireTimerHandle;


private:
	/** The Character holding this weapon*/
	ACHCharacterPlayer* Character;
	FTimerHandle ShootTimerHandle;
	float ShootingPreparationTime = 0.2f;
};
