// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Gun/CHGunBase.h"
#include "CHMinigun.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API ACHMinigun : public ACHGunBase
{
	GENERATED_BODY()

public:
	ACHMinigun();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> CannonMesh1P;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> CannonMesh3P;

protected:
	/* cannon rotate */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> CannonRotateMontage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CHGunBase|Audio")
	TObjectPtr<class USoundCue> CannonRotateSound;
	// USoundCue* CannonRotateSound;
    	
public:
	virtual void Equip() override;
	virtual void UnEquip() override;
	virtual void Fire() override;
	virtual void LocalFire(const FVector& HitLocation,const FTransform& MuzzleTransform) override;
	virtual void PullTriggerByAI(AActor* AttackTarget) override;
	virtual void FireByAI(AActor* AttackTarget) override;
	virtual void AutoFireByAI(AActor* AttackTarget)override;
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

	virtual void OnStartAim() override;
	virtual void OnStopAim() override;
	virtual void OnPullTrigger() override;
	virtual void OnCancelPullTrigger() override;
	void StartRotateCannon();
	void StopRotateCannon();
	
private:
	uint8 bAiming;
	uint8 bShooting;
};
