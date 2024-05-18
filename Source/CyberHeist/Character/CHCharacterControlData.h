// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Camera/CameraComponent.h"
#include "CHCharacterControlData.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UCHCharacterControlData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UCHCharacterControlData();

	// Pawn의 RotationYaw 를 쓸지 말지 결정하는 변수 
	UPROPERTY(EditAnywhere, Category = Pawn)
	uint32 bUseControllerRotationYaw : 1;

	UPROPERTY(EditAnywhere, Category = Pawn)
	uint32 bUseControllerRotationPitch : 1;

	UPROPERTY(EditAnywhere, Category = Pawn)
	uint32 bUseControllerRotationRoll : 1;
	
	// Movement
	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	uint32 bOrientRotationToMovement : 1;

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	uint32 bUseControllerDesiredRotation : 1;

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	FRotator RotationRate;

	// 맵핑
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	// Camera
	UPROPERTY(EditAnywhere, Category = "Third Perspective|Camera")
	FVector TP_CameraPosition;

	UPROPERTY(EditAnywhere, Category = "Third Perspective|Camera")
	uint32 bTPP_UsePawnControlRotation : 1;

	UPROPERTY(EditAnywhere, Category = "Third Perspective|Camera", meta = (ClampMin = "0.0", ClampMax = "180.0", DisplayName = "Third Person FOV", UIMin = "0.0", UIMax = "180.0", Suffix = "°"))
	float TP_FieldOfView;
	
	UPROPERTY(EditAnywhere, Category = "First Perspective|Camera")
	FVector FP_CameraPosition;

	UPROPERTY(EditAnywhere, Category = "First Perspective|Camera")
	uint32 bFPP_UsePawnControlRotation : 1;	
	
	UPROPERTY(EditAnywhere, Category = "First Perspective|Camera", meta = (ClampMin = "0.0", ClampMax = "180.0", DisplayName = "First Person FOV", UIMin = "0.0", UIMax = "180.0", Suffix = "°"))
	float FP_FieldOfView;
	
	// SpringArm
	UPROPERTY(EditAnywhere, Category = "Third Perspective|SpringArm")
	FVector SocketOffset;
	
	UPROPERTY(EditAnywhere, Category = "Third Perspective|SpringArm")
	float TargetArmLength;

	UPROPERTY(EditAnywhere, Category = "Third Perspective|SpringArm")
	FRotator RelativeRotation;

	UPROPERTY(EditAnywhere, Category = "Third Perspective|SpringArm")
	uint32 bUsePawnControlRotation : 1;

	UPROPERTY(EditAnywhere, Category = "Third Perspective|SpringArm")
	uint32 bInheritPitch : 1;

	UPROPERTY(EditAnywhere, Category = "Third Perspective|SpringArm")
	uint32 bInheritYaw : 1;

	UPROPERTY(EditAnywhere, Category = "Third Perspective|SpringArm")
	uint32 bInheritRoll : 1;

	UPROPERTY(EditAnywhere, Category = "Third Perspective|SpringArm")
	uint32 bDoCollisionTest : 1;

};
