// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CHJuggernautAIController.h"

#include "Navigation/CrowdFollowingComponent.h"


#include <ObjectArray.h>

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CHAI.h"
#include "Character/CHCharacterBase.h"
#include "Character/CHCharacterPlayer.h"
#include "Kismet/KismetMathLibrary.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"

ACHJuggernautAIController::ACHJuggernautAIController(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	// Create AIPerceptionComponent
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	if (AIPerception)
	{
		// Setup SightSenseConfig
		UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
		SightConfig->SightRadius = 1000.0f; // Set sight radius as desired
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		AIPerception->ConfigureSense(*SightConfig);

		// Setup SoundSenseConfig
		UAISenseConfig_Hearing* SoundConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("SoundConfig"));
		SoundConfig->HearingRange = 1000.0f; // Set hearing range as desired
		SoundConfig->DetectionByAffiliation.bDetectEnemies = true;
		SoundConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SoundConfig->DetectionByAffiliation.bDetectFriendlies = true;
		AIPerception->ConfigureSense(*SoundConfig);

		// Setup DamageConfig
		UAISenseConfig_Damage* DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
		AIPerception->ConfigureSense(*DamageConfig);
		
		// Set the dominant sense
		AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

		AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ACHJuggernautAIController::HandleSightSense);
		// AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ACHAIController::HandleSoundSense);
		// AIPerception->OnPerceptionUpdated.AddDynamic(this,&ACHAIController::HandleSenses);
	}
}

void ACHJuggernautAIController::RunAI()
{
	Super::RunAI();
}

void ACHJuggernautAIController::StopAI()
{
	Super::StopAI();
}

void ACHJuggernautAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ACHJuggernautAIController::HandleSightSense(AActor* Actor, FAIStimulus Stimulus)
{
	// Check if the stimulus is sight stimulus
	if (Stimulus.Type != UAISense::GetSenseID<UAISense_Sight>()) return;

	// Handle sight stimulus
	// 플레이어가 아니면 제외
	ACHCharacterPlayer* CharacterActor = Cast<ACHCharacterPlayer>(Actor);
	if(CharacterActor == nullptr) return;
	
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	UObject* TargetActor = UKismetMathLibrary::SelectObject(CharacterActor, nullptr, Stimulus.WasSuccessfullySensed());
	
	BlackboardPtr->SetValueAsObject(BBKEY_TARGETACTOR, TargetActor);
	if(Stimulus.WasSuccessfullySensed())
	{
		BlackboardPtr->SetValueAsVector(BBKEY_LASTKNOWNLOCATION, Stimulus.StimulusLocation);
	}
}
