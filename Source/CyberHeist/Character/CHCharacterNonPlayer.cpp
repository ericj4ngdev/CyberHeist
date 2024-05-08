// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CHCharacterNonPlayer.h"

#include "BrainComponent.h"
#include "AI/CHAIController.h"
#include "AI/CHAI.h"
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
	
	ACHAIController* CHAIController = Cast<ACHAIController>(GetController());
    if (CHAIController)
    {
    	CHAIController->StopAI();
    }
	
	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]() {
			TArray<AActor*> AttachedActors;
			GetAttachedActors(AttachedActors,false);
			for (AActor*& AttachedActor : AttachedActors)
			{
				AttachedActor->Destroy();
			}
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

void ACHCharacterNonPlayer::AttackByAI(AActor* Target)
{
	if(Inventory.Weapons.IsEmpty()) return;
	if(CurrentWeapon == nullptr) return;
	
	CurrentWeapon->FireByAI(Target);
	
	// 바로 초기화 말고 공격은 하고 
	// GetWorld()->GetTimerManager().ClearTimer(CurrentWeapon->FireTimerHandle);
	// CurrentWeapon->CancelPullTrigger();
	// UE_LOG(LogTemp, Log, TEXT("ACHCharacterNonPlayer::AttackByAI"));
}

void ACHCharacterNonPlayer::CancelAttackByAI()
{
	if(GetIsAttacking())
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

void ACHCharacterNonPlayer::SetMovementSpeed(ECHAIMovementSpeed Speed)
{
	switch (Speed) {
	case ECHAIMovementSpeed::Idle:
		GetCharacterMovement()->MaxWalkSpeed = 0;
		break;
	case ECHAIMovementSpeed::Walking:
		GetCharacterMovement()->MaxWalkSpeed = 150;
		break;
	case ECHAIMovementSpeed::Jogging:
		GetCharacterMovement()->MaxWalkSpeed = 300;
		break;
	case ECHAIMovementSpeed::Sprinting:
		GetCharacterMovement()->MaxWalkSpeed = 400;
		break;
	}
}

void ACHCharacterNonPlayer::StartAimWeapon()
{
	if(GetCurrentWeapon())
	{
		GetCurrentWeapon()->StartAim();
		SetAiming(true);
	}
}

void ACHCharacterNonPlayer::StopAimWeapon()
{
	if(GetCurrentWeapon())
	{
		GetCurrentWeapon()->StopAim();
		SetAiming(false);
	}
}


void ACHCharacterNonPlayer::Cover()
{
	Crouch();
	// UE_LOG(LogTemp, Log, TEXT("Cover"));
}

void ACHCharacterNonPlayer::UnCover()
{
	UnCrouch();
}
