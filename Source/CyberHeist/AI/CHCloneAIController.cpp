// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CHCloneAIController.h"

#include "Navigation/CrowdFollowingComponent.h"

ACHCloneAIController::ACHCloneAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
}

void ACHCloneAIController::RunAI()
{
	Super::RunAI();
}

void ACHCloneAIController::StopAI()
{
	Super::StopAI();
}

void ACHCloneAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}
