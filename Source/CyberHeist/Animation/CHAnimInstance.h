// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CHAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UCHAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UCHAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	void SetCombatMode(uint8 combat);
	void SetHighCover(uint8 TakeHighCover);
	void SetLowCover(uint8 TakeLowCover);
	void SetCoveredDirection(uint8 bRight);
	void Recoil(float Multiplier);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class ACharacter> Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class UCharacterMovementComponent> CharacterMovement;

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class UNavMovementComponent> NavMovementComponent;*/
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variable)
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variable)
	float GroundSpeed;

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsIdle : 1;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Variable)
	uint8 bIsFalling : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Variable)
	uint8 bIsCrouching : 1;

	// Cover
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Variable)
	uint8 bTakeHighCover : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Variable)
	uint8 bTakeLowCover : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Variable)
	uint8 bCoverMoveRight : 1;

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Threshould)
	float MovingThreshould;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Threshould)
	float JumpingThreshould;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Variable)
	uint8 bCombat : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Variable)
	uint8 bShouldMove : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rotator)
	float Pitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rotator)
	float Roll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rotator)
	float Yaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	FTransform RecoilTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	FTransform RecoilTemp;

};
