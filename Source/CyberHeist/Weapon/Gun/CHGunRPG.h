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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FirstLookAction;
	
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
	FVector SpawnLocation; 
	FRotator SpawnRotation;
public:
	FORCEINLINE FVector GetSpawnLocation() const {return SpawnLocation;}
	// 캐릭터 로테이션 

private:
	void FireActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded);
public:
	virtual void Equip() override;
	virtual void UnEquip() override;
	virtual void Fire() override;
	virtual void EndShoot() override;
	virtual void PullTrigger() override;
	virtual void CancelPullTrigger() override;
	virtual void StartAim() override;
	virtual void StopAim() override;
	virtual void StartPrecisionAim() override;
	virtual void StopPrecisionAim() override;
	virtual void Reload() override;
	virtual void SetupWeaponInputComponent() override;
	void FirstLook(const FInputActionValue& Value);

	virtual void SetWeaponMeshVisibility(uint8 bVisible) override;
	
public:
	virtual void SetOwningCharacter(ACHCharacterBase* InOwningCharacter) override;
	virtual void PickUpOnTouch(ACHCharacterBase* InCharacter) override; 
	virtual void StopParticleSystem() override;
};
