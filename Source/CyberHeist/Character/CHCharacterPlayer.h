// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CHCharacterBase.h"
#include "InputActionValue.h"
#include "Weapon/Gun/CHGun.h"
#include "Interface/CHCharacterHUDInterface.h"
#include "Interface/CHCrossHairWidgetInterface.h"
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

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	virtual void ChangeCharacterControl() override;
	virtual void SetCharacterControl(ECharacterControlType NewCharacterControlType) override;
	virtual void SetCharacterControlData(const class UCHCharacterControlData* CharacterControlData) override;

// Camera Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UCameraComponent> ThirdPersonCamera;

	UPROPERTY(EditAnywhere)
	float AimDistance;

	UPROPERTY(EditAnywhere)
	float DefaultCameraDistance;

	// Input Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ChangeControlAction;

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
	TObjectPtr<class UInputAction> ShootAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ChangeNextWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ChangePrevWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> TakeCoverAction;
	
	/*void Shoot();
	void CancelShoot();
	void StartAim();
	void StopAim();*/

	void FirstMove(const FInputActionValue& Value);
	void FirstLook(const FInputActionValue& Value);
	void ThirdMove(const FInputActionValue& Value);
	void ThirdLook(const FInputActionValue& Value);
	void TakeCover();
	
public:
	void StartSprint();
	void StopSprint();
	 
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCHAnimInstance> CHAnimInstance;
	
	

	// UI Section
protected:
	virtual void SetupHUDWidget(class UCHHUDWidget* InHUDWidget) override;
	virtual void SetupCrossWidget(class UCHUserWidget* InUserWidget) override;

};
