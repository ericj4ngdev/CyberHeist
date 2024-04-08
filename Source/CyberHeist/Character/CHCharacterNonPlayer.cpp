// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CHCharacterNonPlayer.h"
#include "AI/CHAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

ACHCharacterNonPlayer::ACHCharacterNonPlayer()
{
	// GetMesh()->SetHiddenInGame(true);

	AIControllerClass = ACHAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// GetCharacterMovement()->bCanCrouch = true;
}

void ACHCharacterNonPlayer::SetDead()
{
	Super::SetDead();

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]() {
			Destroy();
		}
	), DeadEventDelayTime, false);
}

float ACHCharacterNonPlayer::GetAIPatrolRadius()
{
	return 800.0f;
}

float ACHCharacterNonPlayer::GetAIDetectRange()
{
	return 400.0f;
}

float ACHCharacterNonPlayer::GetAIAttackRange()
{
	return 900.0f;
}

float ACHCharacterNonPlayer::GetAITurnSpeed()
{
	return 100.0f;
}

void ACHCharacterNonPlayer::SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}

void ACHCharacterNonPlayer::AttackByAI()
{
	if(Inventory.Weapons.IsEmpty()) return;
	if(CurrentWeapon == nullptr) return;
	// if(!CurrentWeapon->FireByAI())
	
	CurrentWeapon->FireByAI();
	// 바로 초기화 말고 공격은 하고 
	// GetWorld()->GetTimerManager().ClearTimer(CurrentWeapon->FireTimerHandle);
	// CurrentWeapon->CancelPullTrigger();
	UE_LOG(LogTemp, Log, TEXT("ACHCharacterNonPlayer::AttackByAI"));
}

void ACHCharacterNonPlayer::CancelAttackByAI()
{
	if(CurrentWeapon->FireByAI())
	{
		UE_LOG(LogTemp, Log, TEXT("ACHCharacterNonPlayer::CancelAttackByAI"));
		CurrentWeapon->CancelPullTrigger();
	}
}

void ACHCharacterNonPlayer::NotifyComboActionEnd()
{
	Super::NotifyComboActionEnd();
	OnAttackFinished.ExecuteIfBound();		 // Broadcast UBTTask_Attack return Succeeded
}

void ACHCharacterNonPlayer::Cover()
{
	Crouch();
	UE_LOG(LogTemp, Log, TEXT("Cover"));
}
