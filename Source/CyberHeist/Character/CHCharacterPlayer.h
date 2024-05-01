// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CHCharacterBase.h"
#include "InputActionValue.h"
#include "Weapon/Gun/CHGun.h"
#include "Interface/CHCharacterHUDInterface.h"
#include "Interface/CHCrossHairWidgetInterface.h"
#include "Math/UnrealMathUtility.h"
#include "CHCharacterPlayer.generated.h"



class ACHGun;

/**
 * 
 */
UCLASS()
class CYBERHEIST_API ACHCharacterPlayer : public ACHCharacterBase, public ICHCharacterHUDInterface, public ICHCrossHairWidgetInterface
{
	GENERATED_BODY()
	
public:
	ACHCharacterPlayer();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	virtual void SetCharacterControl(ECharacterControlType NewCharacterControlType) override;
	virtual void SetCharacterControlData(const class UCHCharacterControlData* CharacterControlData) override;
	virtual void SetMappingContextPriority(const UInputMappingContext* MappingContext, int32 Priority) override;
// Camera Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FirstPersonCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> ThirdPersonCamera;
	
	UPROPERTY(EditAnywhere)
	float AimDistance;

	UPROPERTY(EditAnywhere)
	float DefaultCameraDistance;
		
	// Input Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ChangePerspectiveControlAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FirstMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FirstLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ThirdMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ThirdLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ChangeNextWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ChangePrevWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> TakeCoverAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> RightTiltAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LeftTiltAction;
	
	/*void Shoot();
	void CancelShoot();
	void StartAim();
	void StopAim();*/

	virtual void Jump() override;
	
	void FirstMove(const FInputActionValue& Value);
	void FirstLook(const FInputActionValue& Value);
	void ThirdMove(const FInputActionValue& Value);
	void ThirdLook(const FInputActionValue& Value);
	void TakeCover();
	void RightTilt();
	void LeftTilt();
	void StopTilt();

	// Toggles between perspectives
	void TogglePerspective();

	// Sets the perspective
	void SetPerspective(uint8 Is1PPerspective);
	
	// Cover System
public:	
	void SetCoveredAttackMotion(uint8 bAim);

	uint8 bEdge;
	float InputVectorDirectionByCamera;

	// 벽 기준 입력값에 따른 좌우 이동방향
	FVector MoveDirection;
	
	float AngleForDirection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover System", meta = (ClampMin = "0.0", ClampMax = "180.0", DisplayName = "AngleForEscapeCover", UIMin = "0.0", UIMax = "180.0", Suffix = "°"))
	float AngleForEscapeCover;
	
	float RadianForEscapeCover;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover System")
	float CheckCoverSphereRadius;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover System")
	float CheckRange;

	
public:
	void StartSprint();
	void StopSprint();
	 
public:	
	TObjectPtr<class UCHAnimInstance> CHAnimInstance;

	

	// UI Section
protected:
	virtual void SetupHUDWidget(class UCHHUDWidget* InHUDWidget) override;
	virtual void SetupCrossWidget(class UCHUserWidget* InUserWidget) override;

};
