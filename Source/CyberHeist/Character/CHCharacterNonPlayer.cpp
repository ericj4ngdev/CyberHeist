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
	return 0.0f;
}

float ACHCharacterNonPlayer::GetAIDetectRange()
{
	return 0.0f;
}

float ACHCharacterNonPlayer::GetAIAttackRange()
{
	return 0.0f;
}

float ACHCharacterNonPlayer::GetAITurnSpeed()
{
	return 0.0f;
}
