// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Weapon/Gun/CHGunBase.h"
#include "CHGunRPG.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API ACHGunRPG : public ACHGunBase
{
	GENERATED_BODY()

public:
	ACHGunRPG();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> ScopeMesh1P;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> ScopeMesh3P;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> MissileMesh1P;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> MissileMesh3P;

	/** Projectile class to spawn */
	UPROPERTY(EditAnywhere, Category = "CHGunBase|Properties")
	TSubclassOf<class ACHProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> Lens;
	
private:
	FRotator SpawnRotation;

private:
	void FireActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded);
public:
	virtual void Equip() override;
	virtual void UnEquip() override;
	virtual void Fire() override;
	virtual void DisableWeaponInput() override;
	virtual void LocalFire(const FVector& HitLocation,const FTransform& MuzzleTransform) override;
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
};
