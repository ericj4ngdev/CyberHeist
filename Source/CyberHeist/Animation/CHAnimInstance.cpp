// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CHAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
		// bIsIdle = GroundSpeed < MovingThreshould;
		bIsFalling = Movement->IsFalling();
		// bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshould);
		bShouldMove = (GroundSpeed > 3) && (Movement->GetCurrentAcceleration().Length() > 0);
	}

	/*FRotator Rotator = UKismetMathLibrary::NormalizedDeltaRotator(Owner->GetBaseAimRotation(), Owner->GetActorRotation());
	Roll = Rotator.Roll;
	Pitch = Rotator.Pitch;
	Yaw = Rotator.Yaw;*/
}

void UCHAnimInstance::SetCombatMode(bool combat)
{
	bCombat = combat;
}
