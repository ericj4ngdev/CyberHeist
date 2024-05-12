// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CHCharacterBase.h"
#include "Interface/CHCharacterAIInterface.h"
#include "CHCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API ACHCharacterNonPlayer : public ACHCharacterBase, public ICHCharacterAIInterface
{
	GENERATED_BODY()
	
public:
	ACHCharacterNonPlayer();

public:
	UFUNCTION(BlueprintCallable)
	virtual void SetDead() override;
	
	// AI Section
protected:
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;
	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) override;
	UFUNCTION(BlueprintCallable)
	virtual void AttackByAI(AActor* Target) override;
	UFUNCTION(BlueprintCallable)
	virtual void AutoAttackByAI(AActor* Target) override;
	UFUNCTION(BlueprintCallable)
	virtual void CancelAttackByAI() override;
	virtual void NotifyComboActionEnd() override;

	UFUNCTION(BlueprintCallable)
	virtual void SetMovementSpeed(ECHAIMovementSpeed Speed) override;

	UFUNCTION(BlueprintCallable)
	virtual void StartAimWeapon() override;

	UFUNCTION(BlueprintCallable)
	virtual void StopAimWeapon() override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void Cover(bool High, bool Right) override;

	UFUNCTION(BlueprintCallable)
	virtual void UnCover() override;
	
	FAICharacterAttackFinished OnAttackFinished;
};
