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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> CannonMesh1P;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> CannonMesh3P;

public:
	virtual void Equip() override;
	virtual void UnEquip() override;
	virtual void Fire() override;
	virtual void PullTrigger() override;
	virtual void CancelPullTrigger() override;
	virtual void StartAim() override;
	virtual void StopAim() override;
	virtual void StartPrecisionAim() override;
	virtual void StopPrecisionAim() override;
	virtual void Reload() override;

public:
	virtual void SetOwningCharacter(ACHCharacterBase* InOwningCharacter) override;
	virtual void PickUpOnTouch(ACHCharacterBase* InCharacter) override; 
	virtual void StopParticleSystem() override;
	
};
