// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CHCharacterNonPlayer.h"

ACHCharacterNonPlayer::ACHCharacterNonPlayer()
{
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
