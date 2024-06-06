// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CHCharacterNonPlayer.h"

#include "BrainComponent.h"
#include "CHCharacterMovementComponent.h"
#include "CyberHeist.h"
#include "AI/CHAIControllerBase.h"
#include "AI/CHAI.h"
#include "Animation/CHAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ACHCharacterNonPlayer::ACHCharacterNonPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// GetMesh()->SetHiddenInGame(true);

	AIControllerClass = ACHAIControllerBase::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// GetCharacterMovement()->bCanCrouch = true;
}

void ACHCharacterNonPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	FVector Start = GetActorUpVector() * 10.0f + GetActorLocation();
	FVector End = GetActorForwardVector() * 50.0f + Start;
	DrawDebugDirectionalArrow(GetWorld(),Start, End, 10.0f, FColor::Red, false, -1, 0 ,10.0f);
}

void ACHCharacterNonPlayer::SetDead()
{
	Super::SetDead();
	
	ACHAIControllerBase* CHAIController = Cast<ACHAIControllerBase>(GetController());
    if (CHAIController)
    {
    	CHAIController->StopAI();
    }
	CancelAttackByAI();
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
	// CurrentWeapon->PullTriggerByAI(Target);
	// 바로 초기화 말고 공격은 하고 
	// GetWorld()->GetTimerManager().ClearTimer(CurrentWeapon->FireTimerHandle);
	// CurrentWeapon->CancelPullTrigger();
	// UE_LOG(LogTemp, Log, TEXT("ACHCharacterNonPlayer::AttackByAI"));
}

void ACHCharacterNonPlayer::AutoAttackByAI(AActor* Target)
{
	if(Inventory.Weapons.IsEmpty()) return;
	if(CurrentWeapon == nullptr) return;
	CurrentWeapon->PullTriggerByAI(Target);
}

void ACHCharacterNonPlayer::CancelAttackByAI()
{
	if(GetIsAttacking())
	{
		UE_LOG(LogTemp, Log, TEXT("ACHCharacterNonPlayer::CancelAttackByAI"));
		CurrentWeapon->EndShoot();
		// CurrentWeapon->CancelPullTrigger();
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
		CH_LOG(LogCHAI, Log, TEXT("Begin"))
		GetCurrentWeapon()->StartAim();
		SetAiming(true);
		CH_LOG(LogCHAI, Log, TEXT("End"))
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


void ACHCharacterNonPlayer::Cover(bool High, bool Right)
{
	// UE_LOG(LogTemp, Log, TEXT("Cover"));
	if(High)
	{
		// 벽 수직 정보? 가져오면 그에 맞게 엄폐?
		CHAnimInstance->SetCoveredDirection(Right);
		OnCoverState.Broadcast(High,true);
		bCovered = true;
	}
	else
	{
		CHAnimInstance->SetCoveredDirection(Right);
		OnCoverState.Broadcast(High, true);
		Crouch();
		UE_LOG(LogTemp, Log, TEXT("AI Low Covered"));
		bCovered = true;
	}
}

void ACHCharacterNonPlayer::TakeCover(FCover Cover)
{
	// MulticastTakeCover(Cover);
	OnTakeCover(Cover);
}

void ACHCharacterNonPlayer::OnTakeCover(FCover Cover)
{
	CH_LOG(LogCHAI, Log, TEXT("Begin"))
	SetActorLocation(Cover.Data.Location);
	SetActorRotation(Cover.Data.Rotation);
	bCovered = true;
	
	if(Cover.Data.bLeftCoverStanding)
	{
		bCoverMoveRight = false;
		bHighCovered = true;
		bLowCovered = true;
		// CHAnimInstance->SetCoveredDirection(false);
		// OnCoverState.Broadcast(true,true);
		CH_LOG(LogCHAI, Log, TEXT("bLeftCoverStanding"))
		return;
	}
	
	if(Cover.Data.bRightCoverStanding)
	{
		bCoverMoveRight = true;
		bHighCovered = true;
		bLowCovered = true;
		// CHAnimInstance->SetCoveredDirection(true);
		// OnCoverState.Broadcast(true,true);
		CH_LOG(LogCHAI, Log, TEXT("bRightCoverStanding"))
		return;
	}

	if(Cover.Data.bLeftCoverCrouched)
	{
        bCoverMoveRight = false;
		bHighCovered = false;
        bLowCovered = true;
		// CHAnimInstance->SetCoveredDirection(false);
		// OnCoverState.Broadcast(false,true);
		Crouch();
		CH_LOG(LogCHAI, Log, TEXT("bLeftCoverCrouched"))
		return;
	}
	
	if(Cover.Data.bRightCoverCrouched)
	{
		bCoverMoveRight = true;
		bHighCovered = false;
		bLowCovered = true;
		// CHAnimInstance->SetCoveredDirection(true);
		// OnCoverState.Broadcast(false,true);
		Crouch();
		CH_LOG(LogCHAI, Log, TEXT("bRightCoverCrouched"))
		return;
	}
	
	if(Cover.Data.bFrontCoverCrouched)
	{
		bCoverMoveRight = false;
		bHighCovered = false;
		bLowCovered = true;
		// CHAnimInstance->SetCoveredDirection(false);
		// OnCoverState.Broadcast(false,true);
		Crouch();
		CH_LOG(LogCHAI, Log, TEXT("bFrontCoverCrouched"))
	}
}

void ACHCharacterNonPlayer::MulticastTakeCover_Implementation(FCover Cover)
{
	OnTakeCover(Cover);
}

void ACHCharacterNonPlayer::UnCoverAim(FCover Cover)
{
	// MulticastUnCoverAim(Cover);
	OnUnCoverAim(Cover);
}

void ACHCharacterNonPlayer::OnUnCoverAim(FCover Cover)
{
	// 이미 uncrouch함.
	UE_LOG(LogTemp,Log,TEXT("ACHCharacterNonPlayer::UnCoverAim"));
	StartAimWeapon();
	UE_LOG(LogTemp,Log, TEXT("%s"), *GetActorLocation().ToString());
	if(Cover.Data.bLeftCoverStanding)
	{
		// 얼마나 나올지를 플레이어와의 거리로 계산 가능?
		SetActorLocation(GetActorLocation() - GetActorRightVector() * GetCapsuleComponent()->GetScaledCapsuleRadius() * 2);
		CH_LOG(LogCHAI, Log, TEXT("bLeftCoverStanding"))
	}
	else if (Cover.Data.bRightCoverStanding)
	{
		SetActorLocation(GetActorLocation() + GetActorRightVector() * GetCapsuleComponent()->GetScaledCapsuleRadius()*2);
		CH_LOG(LogCHAI, Log, TEXT("bRightCoverStanding"))
	}
	// 낮은 경우는 그냥 일어나면 되어서 패스.
}

void ACHCharacterNonPlayer::MulticastUnCoverAim_Implementation(FCover Cover)
{
	OnUnCoverAim(Cover);
}

void ACHCharacterNonPlayer::UnCover()
{	
	UnCrouch();
	bCovered = false;
}
