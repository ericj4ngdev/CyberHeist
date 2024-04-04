// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CHAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"	
#include "Character/CHCharacterPlayer.h"
#include "Character/CHCharacterBase.h"
#include "Kismet/GameplayStatics.h"

UCHAnimInstance::UCHAnimInstance()
{
	// MovingThreshould = 3.0f;
	// JumpingThreshould = 100.0f;
}

void UCHAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void UCHAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement)
	{
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D();
		bIsFalling = Movement->IsFalling();
		FVector temp = Movement->GetCurrentAcceleration();
		// TODO: Compare Acceleration with Zero Vector 
		bShouldMove = (GroundSpeed > 3);		
		//&& !temp.Equals(FVector::Zero(), 0.1f);
	}
	
	ACHCharacterBase* OwnerActor = Cast<ACHCharacterBase>(Owner);
	if (OwnerActor)
	{
		// OwnerActor->OnCombat.AddUObject(this, &UCHAnimInstance::SetCombatMode);
		SetCombatMode(OwnerActor->GetCombatMode());
	}
	
	RecoilTemp = UKismetMathLibrary::TInterpTo(RecoilTemp, RecoilTransform, UGameplayStatics::GetWorldDeltaSeconds(this), 25.0f);

	RecoilTransform = UKismetMathLibrary::TInterpTo(RecoilTransform, FTransform(), UGameplayStatics::GetWorldDeltaSeconds(this), 15.0f);

	/*FRotator Rotator = UKismetMathLibrary::NormalizedDeltaRotator(Owner->GetBaseAimRotation(), Owner->GetActorRotation());
	Roll = Rotator.Roll;
	Pitch = Rotator.Pitch;
	Yaw = Rotator.Yaw;*/
}

void UCHAnimInstance::SetCombatMode(uint8 combat)
{
	bCombat = combat;
}

void UCHAnimInstance::Recoil(float Multiplier)
{
	float LocalMultiplier;
	FVector RecoilLocation;
	FRotator RecoilRotation;
	FQuat RecoilRotationQuat; // FRotator 대신 FQuat을 사용합니다.

	float RRoll = UKismetMathLibrary::RandomFloatInRange(-2.5f, -5.0f);
	float RPitch = UKismetMathLibrary::RandomFloatInRange(-0.8f, -0.8f);
	float RYaw = UKismetMathLibrary::RandomFloatInRange(-1.6f, -1.6f);

	LocalMultiplier = Multiplier;
	// RecoilRotation = RecoilTransform.Rotator();
	// RecoilRotation = FRotator(RRoll, RPitch, RYaw) * LocalMultiplier;
	RecoilRotationQuat = FQuat(FRotator(RRoll, RPitch, RYaw)) * LocalMultiplier; // FRotator를 FQuat으로 변환합니다.

	float RX = UKismetMathLibrary::RandomFloatInRange(-0.16f, 0.16f);
	float RY = UKismetMathLibrary::RandomFloatInRange(-1.1f, -2.1f);
	float RZ = UKismetMathLibrary::RandomFloatInRange(-0.0f, -0.0f);

	RecoilLocation = RecoilTransform.GetLocation();
	RecoilLocation = FVector(RX, RY, RZ) * LocalMultiplier;

	RecoilTransform.SetLocation(RecoilLocation);
	RecoilTransform.SetRotation(RecoilRotationQuat);
}