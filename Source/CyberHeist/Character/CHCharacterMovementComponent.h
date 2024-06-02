// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CHCharacterMovementComponent.generated.h"


class FCHSavedMove_Character : public FSavedMove_Character
{
	typedef FSavedMove_Character Super;
public:
	virtual void Clear() override;
	virtual void SetInitialPosition(ACharacter* Character) override; //최초 위치, 최초 상태를 저장 
	virtual uint8 GetCompressedFlags() const override;

	uint8 bPressedSprint : 1; //입력 전달
	uint8 bPressedAiming : 1;
	uint8 bCovered : 1;
	uint8 bHoldMinigun : 1;

	float WalkSpeed;
	float RunSpeed;
	float SneakSpeed;
};

class FCHNetworkPredictionData_Client_Character : public FNetworkPredictionData_Client_Character
{
	typedef FNetworkPredictionData_Client_Character Super;
public:
	FCHNetworkPredictionData_Client_Character(const UCharacterMovementComponent &ClientMovement);

	virtual FSavedMovePtr AllocateNewMove() override;
};

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UCHCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UCHCharacterMovementComponent();

	void SetSprintCommand(uint8 bNewSprint);
	void SetAimingCommand(uint8 bNewAiming);
	void SetCovered(uint8 bNewCovered);
	void SetCurrentGun(uint8 bNewHoldMinigun);
	
protected:
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	
	void SetSpeed();

public:
	uint8 bPressedSprint : 1;
	uint8 bPressedAiming : 1;
	uint8 bCovered : 1;
	uint8 bHoldMinigun : 1;

	float WalkSpeed;
	float RunSpeed;
	float SneakSpeed;
};
