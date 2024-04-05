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
	virtual void SetDead() override;

	// AI Section
protected:
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;
	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) override;
	virtual void AttackByAI() override;
	virtual void CancelAttackByAI() override;
	virtual void NotifyComboActionEnd() override;
	
	FAICharacterAttackFinished OnAttackFinished;
};
