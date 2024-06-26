// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CHCharacterBase.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "Weapon/Gun/CHGun.h"
#include "Interface/CHCharacterHUDInterface.h"
#include "Interface/CHCrossHairWidgetInterface.h"
#include "Math/UnrealMathUtility.h"
#include "CHCharacterPlayer.generated.h"


/**
 * 
 */
UCLASS()
class CYBERHEIST_API ACHCharacterPlayer : public ACHCharacterBase, public ICHCharacterHUDInterface, public ICHCrossHairWidgetInterface
{
	GENERATED_BODY()
	
public:
	ACHCharacterPlayer(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void PostNetInit() override;
	virtual void OnRep_Owner() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	virtual void SetCharacterControl(ECharacterControlType NewCharacterControlType) override;
	virtual void SetCharacterControlData(const class UCHCharacterControlData* CharacterControlData) override;
	virtual void SetMappingContextPriority(const UInputMappingContext* MappingContext, int32 Priority) override;

public:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_SetCharacterControl(ECharacterControlType NewCharacterControlType);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_SetCharacterControl(ECharacterControlType NewCharacterControlType);

	void CoverEnd(UAnimMontage* AnimMontage, bool bArg);


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

public:
	UCameraComponent* GetThirdPersonCamera() const{ return ThirdPersonCamera;}
	UCameraComponent* GetFirstPersonCamera() const{ return FirstPersonCamera;}
	USpringArmComponent* GetSpringArm() const {return CameraBoom;}
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CrouchAction;
	
	/*void Shoot();
	void CancelShoot();
	void StartAim();
	void StopAim();*/

	virtual void Jump() override;
	
	void FirstMove(const FInputActionValue& Value);
	void FirstLook(const FInputActionValue& Value);
	void ThirdMove(const FInputActionValue& Value);
	void ThirdLook(const FInputActionValue& Value);

	void LocalCoveredMove(const FInputActionValue& Value);
	
	void TakeCover();
	void StartCover();
	void StopCover();
	
	void TakeCrouch();
	void StartCrouch();
	void StopCrouch();

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerStartCoverMotion(const FTransform& Destination);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastStartCoverMotion(const FTransform& Destination);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FRotator LastCoveredRotation;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetCoveredRotation(FRotator NewCoveredRotation);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CharacterHalfHeight;
	
	void ReturnCover();
protected:
	UFUNCTION()
	void SetTiltingRightValue(float Output);
	UFUNCTION()
	void SetTiltingLeftValue(float Output);
	void TiltRight();
	void TiltRightRelease();
	void TiltLeft();
	void TiltLeftRelease();
	void PressV();
	// void StopTilt(const float Value);

	UPROPERTY(EditAnywhere, Category = "Tilt System") // Timeline 생성
	FTimeline TiltingLeftTimeline;
	
	UPROPERTY(EditAnywhere, Category = "Tilt System") // Timeline 생성
	FTimeline TiltingRightTimeline;

	UPROPERTY(EditAnywhere, Category = "Tilt System") // Timeline 커브
	TObjectPtr<UCurveFloat> TiltingCurveFloat;

	FVector CameraCurrentPosition;
	FVector CameraDesiredPosition;
	FRotator CameraCurrentRotation;
	FRotator CameraDesiredRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Tilt System")
	uint8 bTiltLeft : 1;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Tilt System")
	uint8 bTiltRight : 1;

	uint8 bFreeLook : 1;
	
	// Toggles between perspectives
	void TogglePerspective();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_SetPerspective(uint8 Is1PPerspective);

	/*UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_SetPerspective(bool Is1PPerspective);*/

	// Sets the perspective
	void SetPerspective(uint8 Is1PPerspective);
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category = Camera)
	uint8 bIsFirstPersonPerspective : 1;

public:
	uint8 IsInFirstPersonPerspective() const{return bIsFirstPersonPerspective;}
	// Cover System
public:	
	void SetCoveredAttackMotion(uint8 bAim);
	
	UFUNCTION(Server, Unreliable)
	void ServerSetCoveredAttackMotion(uint8 bAim);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetCoveredAttackMotion(uint8 bAim);

	float InputVectorDirectionByCamera;

	// 벽 기준 입력값에 따른 좌우 이동방향
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FVector MoveDirection;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetMoveDirection(FVector NewMoveDirection);	

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetActorRotation(FRotator NewRotator);
	
	float AngleForDirection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover System", meta = (ClampMin = "0.0", ClampMax = "180.0", DisplayName = "AngleForEscapeCover", UIMin = "0.0", UIMax = "180.0", Suffix = "°"))
	float AngleForEscapeCover;
	
	float RadianForEscapeCover;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover System")
	float CheckCoverSphereRadius;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover System")
	float CheckRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Cover System")
	uint8 bMovetoCover;
	
public:
	UFUNCTION(BlueprintCallable)
	virtual void SetDead() override;
	
public:
	void PressSprint();
	void ReleaseSprint();
	virtual void StartSprint() override;
	virtual void StopSprint() override;

	// UI Section
protected:
	virtual void SetupHUDWidget(class UCHHUDWidget* InHUDWidget) override;
	virtual void SetupCrossWidget(class UCHUserWidget* InUserWidget) override;

public:
	void SetSpawnPoint(const FTransform& SpawnTransform);
};
