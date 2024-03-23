// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CHAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/CHCharacterPlayer.h"

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

	ACHCharacterPlayer* OwnerActor = Cast<ACHCharacterPlayer>(Owner);
	if (OwnerActor)
	{
		// OwnerActor->OnCombat.AddUObject(this, &UCHAnimInstance::SetCombatMode);

	}

}

void UCHAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement)
	{
		// Velocity = Owner->
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D();
		// bIsIdle = GroundSpeed < MovingThreshould;
		bIsFalling = Movement->IsFalling();
		// bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshould);
		bShouldMove = (GroundSpeed > 3) && (Movement->GetCurrentAcceleration().Length() > 0);
	}

	ACHCharacterPlayer* OwnerActor = Cast<ACHCharacterPlayer>(Owner);
	if (OwnerActor)
	{
		// OwnerActor->OnCombat.AddUObject(this, &UCHAnimInstance::SetCombatMode);
		SetCombatMode(OwnerActor->GetCombatMode());
	}

	/*FRotator Rotator = UKismetMathLibrary::NormalizedDeltaRotator(Owner->GetBaseAimRotation(), Owner->GetActorRotation());
	Roll = Rotator.Roll;
	Pitch = Rotator.Pitch;
	Yaw = Rotator.Yaw;*/
}

void UCHAnimInstance::SetCombatMode(uint8 combat)
{
	bCombat = combat;
}

void UCHAnimInstance::GetCombatMode()
{
	/*ACHCharacterPlayer* OwnerActor = Cast<ACHCharacterPlayer>(GetOwningActor());
	if (OwnerActor)
	{
		OwnerActor->OnCombat.AddUObject(this, &UCHAnimInstance::SetCombatMode);
	}*/
}
