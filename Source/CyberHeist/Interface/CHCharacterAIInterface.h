// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AI/CHAI.h"
#include "CHCharacterAIInterface.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCHCharacterAIInterface : public UInterface
{
	GENERATED_BODY()
};

DECLARE_DELEGATE(FAICharacterAttackFinished);
/**
 * 
 */
class CYBERHEIST_API ICHCharacterAIInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual float GetAIPatrolRadius() = 0;
	virtual float GetAIDetectRange() = 0;
	virtual float GetAIAttackRange() = 0;
	virtual float GetAITurnSpeed() = 0;

	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) = 0;
	virtual void AttackByAI(AActor* Target) = 0;
	virtual void CancelAttackByAI() = 0;
	virtual void Cover() = 0;
	virtual void UnCover() = 0;
	virtual void SetMovementSpeed(ECHAIMovementSpeed Speed)=0;
	virtual void StartAimWeapon() = 0;
	virtual void StopAimWeapon() = 0;
};
