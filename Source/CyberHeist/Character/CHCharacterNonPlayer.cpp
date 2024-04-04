// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CHCharacterNonPlayer.h"
#include "AI/CHAIController.h"

ACHCharacterNonPlayer::ACHCharacterNonPlayer()
{
	// GetMesh()->SetHiddenInGame(true);

	AIControllerClass = ACHAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
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
	return 2.0f;
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
