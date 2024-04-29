// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	virtual void SetupWeaponInputComponent() override;
public:
	virtual void SetOwningCharacter(ACHCharacterBase* InOwningCharacter) override;
	virtual void PickUpOnTouch(ACHCharacterBase* InCharacter) override; 
	virtual void StopParticleSystem() override;

};
