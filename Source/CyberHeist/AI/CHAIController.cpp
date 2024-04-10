// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CHAIController.h"

#include <ObjectArray.h>

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CHAI.h"
#include "Character/CHCharacterBase.h"
#include "GameFramework/Character.h"
#include "KisMet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"

ACHAIController::ACHAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAssetRef(TEXT("/Script/AIModule.BlackboardData'/Game/CyberHeist/AI/BB_CHCharacter.BB_CHCharacter'"));
	if (nullptr != BBAssetRef.Object)
	{
		BBAsset = BBAssetRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAssetRef(TEXT("/Script/AIModule.BehaviorTree'/Game/CyberHeist/AI/BT_CHCharacter.BT_CHCharacter'"));
	if (nullptr != BTAssetRef.Object)
	{
		BTAsset = BTAssetRef.Object;
	}

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

		// Set the dominant sense
		AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

		AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ACHAIController::HandleSightSense);
	}
	
}

void ACHAIController::RunAI()
{
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	if (UseBlackboard(BBAsset, BlackboardPtr))
	{
		Blackboard->SetValueAsVector(BBKEY_HOMEPOS, GetPawn()->GetActorLocation());
		
		bool RunResult = RunBehaviorTree(BTAsset);
		
		ensure(RunResult);
	}
}

void ACHAIController::StopAI()
{
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComponent)
	{
		BTComponent->StopTree();  // 중지
	}
}

void ACHAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	RunAI();
}

void ACHAIController::HandleSightSense(AActor* Actor, FAIStimulus Stimulus)
{
	// Check if the stimulus is sight stimulus
	if (Stimulus.Type != UAISense::GetSenseID<UAISense_Sight>()) return;

	// Handle sight stimulus
	// ACharacter* Character = Cast<ACharacter>(Actor);
	// APawn* ControllingPawn = SetPawn();
	ACHCharacterBase* CharacterActor = Cast<ACHCharacterBase>(Actor);
	if(CharacterActor == nullptr) return;
	// if(Character != UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) return;
	
	
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	UObject* TargetActor = UKismetMathLibrary::SelectObject(CharacterActor, nullptr, Stimulus.WasSuccessfullySensed());
	BlackboardPtr->SetValueAsObject(BBKEY_TARGETACTOR, TargetActor);
	if(Stimulus.WasSuccessfullySensed())
	{
		BlackboardPtr->SetValueAsVector(BBKEY_LASTKNOWNLOCATION, Stimulus.StimulusLocation);
	}
	
}

