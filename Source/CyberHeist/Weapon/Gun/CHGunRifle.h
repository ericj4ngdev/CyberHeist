// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Weapon/Gun/CHGunBase.h"
#include "CHGunRifle.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API ACHGunRifle : public ACHGunBase
{
	GENERATED_BODY()
	
public:
	ACHGunRifle();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
public:
	virtual void Equip() override;
	virtual void UnEquip() override;
	virtual void DisableWeaponInput() override;
	virtual void Fire() override;
	virtual void LocalFire(const FVector& HitLocation,const FTransform& MuzzleTransform) override;
	virtual void PullTriggerByAI(AActor* AttackTarget) override;
	virtual void FireByAI(AActor* AttackTarget) override;
	virtual void AutoFireByAI(AActor* AttackTarget) override;
	virtual void EndShoot() override;
	virtual void PullTrigger() override;
	virtual void CancelPullTrigger() override;
	virtual void StartAim() override;
	virtual void StopAim() override;
	virtual void StartPrecisionAim() override;
	virtual void StopPrecisionAim() override;
	virtual void StayPrecisionAim() override;
	virtual void Reload() override;
	virtual void SetupWeaponInputComponent() override;

	virtual void SetWeaponMeshVisibility(uint8 bVisible) override;
public:
	virtual void SetOwningCharacter(ACHCharacterBase* InOwningCharacter) override;
	virtual void StopParticleSystem() override;
	virtual void PlayFireVFX(const FTransform& HitTransform, const FTransform& MuzzleTransform) override;
		
	UPROPERTY()
	TObjectPtr<class AActor> AttackTargetActor;
};
