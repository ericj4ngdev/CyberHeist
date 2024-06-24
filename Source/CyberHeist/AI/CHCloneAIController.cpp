// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CHCloneAIController.h"
#include "CHPathFollowingComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "Navigation/CrowdFollowingComponent.h"

ACHCloneAIController::ACHCloneAIController(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAssetRef(TEXT("/Script/AIModule.BlackboardData'/Game/CyberHeist/AI/BehaviorTrees/BB_CHClone.BB_CHClone'"));
	if (nullptr != BBAssetRef.Object)
	{
		BBAsset = BBAssetRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAssetRef(TEXT("/Script/AIModule.BehaviorTree'/Game/CyberHeist/AI/BehaviorTrees/BT_CHClone.BT_CHClone'"));
	if (nullptr != BTAssetRef.Object)
	{
		BTAsset = BTAssetRef.Object;
	}
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
