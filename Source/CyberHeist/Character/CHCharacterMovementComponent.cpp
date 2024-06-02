// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CHCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "CyberHeist.h"

UCHCharacterMovementComponent::UCHCharacterMovementComponent()
{
	bPressedSprint = false;
	bPressedAiming = false;
	bCovered = false;
	bHoldMinigun = false;
	WalkSpeed = 200;
	RunSpeed = 450;
	SneakSpeed = 100;
}

void UCHCharacterMovementComponent::SetSprintCommand(uint8 bNewSprint)
{
	bPressedSprint = bNewSprint;
}

void UCHCharacterMovementComponent::SetAimingCommand(uint8 bNewAiming)
{
	bPressedAiming = bNewAiming;
}

void UCHCharacterMovementComponent::SetCovered(uint8 bNewCovered)
{
	bCovered = bNewCovered;
}

void UCHCharacterMovementComponent::SetCurrentGun(uint8 bNewHoldMinigun)
{
	bHoldMinigun = bNewHoldMinigun;
}

FNetworkPredictionData_Client* UCHCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UCHCharacterMovementComponent* MutableThis = const_cast<UCHCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FCHNetworkPredictionData_Client_Character(*this);
	}
	return ClientPredictionData;
}

void UCHCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	SetSpeed();
}

void UCHCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	// 서버가 받은 Flag로 움직임 구현(서버 캐릭)
	// 디코딩 작업 
	Super::UpdateFromCompressedFlags(Flags);
	
	bPressedSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	bPressedAiming = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
	bCovered = (Flags & FSavedMove_Character::FLAG_Custom_2) != 0;
	bHoldMinigun = (Flags & FSavedMove_Character::FLAG_Custom_3) != 0;

	if (CharacterOwner && CharacterOwner->GetLocalRole() == ROLE_Authority)
	{
		SetSpeed();
	}
}

void UCHCharacterMovementComponent::SetSpeed()
{
	MaxWalkSpeed = bPressedSprint ? RunSpeed : WalkSpeed;
	if(bPressedAiming) MaxWalkSpeed = WalkSpeed; 
	if(bCovered) MaxWalkSpeed = SneakSpeed;
	if(bHoldMinigun) MaxWalkSpeed = SneakSpeed;	
}

void FCHSavedMove_Character::Clear()
{
	Super::Clear();

	bPressedSprint = false;
	bPressedAiming = false;
	bCovered = false;
	bHoldMinigun = false;

	WalkSpeed = 200;
	RunSpeed = 450;
	SneakSpeed = 100;
}

void FCHSavedMove_Character::SetInitialPosition(ACharacter* Character)
{
	Super::SetInitialPosition(Character);
	
	UCHCharacterMovementComponent* CHMovement = Cast<UCHCharacterMovementComponent>(Character->GetCharacterMovement());
	if(CHMovement)
	{
		bPressedSprint = CHMovement->bPressedSprint;
		bPressedAiming = CHMovement->bPressedAiming;
		bHoldMinigun = CHMovement->bHoldMinigun;
		bCovered = CHMovement->bCovered;	
	}
}

uint8 FCHSavedMove_Character::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	
	// 여기에 압축해서 서버로 보내주기
	if(bPressedSprint)
	{
		Result |= FLAG_Custom_0;
	}
	if(bPressedAiming)
	{
		Result |= FLAG_Custom_1;
	}
	if(bCovered)
	{
		Result |= FLAG_Custom_2;
	}
	if(bHoldMinigun)
	{
		Result |= FLAG_Custom_3;
	}

	return Result;
}


FCHNetworkPredictionData_Client_Character::FCHNetworkPredictionData_Client_Character(
	const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{
}

FSavedMovePtr FCHNetworkPredictionData_Client_Character::AllocateNewMove()
{
	return FSavedMovePtr(new FCHSavedMove_Character());
}
