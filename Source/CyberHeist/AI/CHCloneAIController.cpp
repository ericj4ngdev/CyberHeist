// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CHCloneAIController.h"
#include "CHPathFollowingComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Character/CHCharacterPlayer.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

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

	/*AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	if (AIPerception)
	{
		// Setup SightSenseConfig
		UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
		SightConfig->SightRadius = 1000.0f; // Set sight radius as desired
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		AIPerception->ConfigureSense(*SightConfig);

		AIPerception->OnPerceptionUpdated.AddDynamic(this,&ACHCloneAIController::HandleSightSenseArray);
	}*/
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

void ACHCloneAIController::HandleSightSenseArray(const TArray<AActor*>& Actors)
{
	RecognizedPlayers.Empty(); // Clear the previous recognized players
	for (AActor* Actor : Actors)
	{
		if (ACHCharacterPlayer* Player = Cast<ACHCharacterPlayer>(Actor))
		{
			RecognizedPlayers.Add(Player);
		}
	}
	
	/*FVector AILocation = GetPawn()->GetActorLocation();

	AActor* NearestActor = nullptr;
	float MinDistance = FLT_MAX;

	for(AActor* Actor : Actors)
	{
		if(Cast<ACHCharacterPlayer>(Actor))
		{
			FVector TargetLocation = Actor->GetActorLocation();
			float Distance = FVector::Dist(AILocation, TargetLocation);

			if(Distance < MinDistance)
			{
				MinDistance = Distance;
				NearestActor = Actor;
			}
		}
	}
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	BlackboardPtr->SetValueAsObject(BBKEY_TARGETACTOR, NearestActor);*/
}