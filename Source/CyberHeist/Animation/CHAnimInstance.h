// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Weapon/Gun/CHGunBase.h"
#include "CHAnimInstance.generated.h"

UENUM(BlueprintType)
enum class ECoverState : uint8
{
	Low		UMETA(DisplayName = "Low"),
	High	UMETA(DisplayName = "High"),
	None	UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Fist				UMETA(DisplayName = "Fist Idle"),
	FistAiming			UMETA(DisplayName = "Fist Aiming"),
	Rifle				UMETA(DisplayName = "Rifle Idle"),
	RifleAiming			UMETA(DisplayName = "Rifle Aiming"),
	RifleScopeAiming	UMETA(DisplayName = "Rifle Scope Aiming"),
	MiniGun				UMETA(DisplayName = "MiniGun Idle"),
	MiniGunAiming		UMETA(DisplayName = "MiniGun Aiming"),
	RPG					UMETA(DisplayName = "RPG Idle"),
	RPGAiming			UMETA(DisplayName = "RPG Aiming"),
	RPGScopeAiming		UMETA(DisplayName = "RPG Scope Aiming"),
};

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
	void SetHighCover(uint8 TakeHighCover);
	void SetLowCover(uint8 TakeLowCover);
	void SetCoverState(uint8 TakeHighCover, uint8 TakeLowCover);
	void SetCoveredDirection(uint8 bRight);
	void Recoil(float Multiplier);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class ACharacter> Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class UCharacterMovementComponent> CharacterMovement;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variable)
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variable)
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Variable)
	uint8 bIsFalling : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Variable)
	uint8 bIsCrouching : 1;

	// Cover
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CoverSystem)
	uint8 bTakeHighCover : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CoverSystem)
	uint8 bTakeLowCover : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CoverSystem)
	uint8 bCoverMoveRight : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CoverSystem)
	float CoverHeight;

	// Aim
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Aim)
	uint8 bAiming : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Aim)
	uint8 bPrecisionAiming : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Aim)
	uint8 bScopeAiming : 1;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Variable)
	uint8 bShouldMove : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rotator)
	float Pitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rotator)
	float Roll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rotator)
	float Yaw;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	FTransform RecoilTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	FTransform RecoilTemp;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CoverSystem)
	ECoverState CurrentCoverState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	EWeaponState CurrentWeaponState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	ECHWeaponType CurrentWeapon;
	
public:
	FORCEINLINE ECoverState GetCurrentCoverState() const {return CurrentCoverState;}
};
