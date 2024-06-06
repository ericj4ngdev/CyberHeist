// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CHAIControllerBase.h"

#include <ObjectArray.h>

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CHAI.h"
#include "Character/CHCharacterBase.h"
#include "Character/CHCharacterPlayer.h"
#include "Kismet/KismetMathLibrary.h"

#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"

ACHAIControllerBase::ACHAIControllerBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	// Create AIPerceptionComponent
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

		AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ACHAIControllerBase::HandleSightSense);
		// AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ACHAIController::HandleSoundSense);
		// AIPerception->OnPerceptionUpdated.AddDynamic(this,&ACHAIController::HandleSenses);
	}*/
	
}

void ACHAIControllerBase::RunAI()
{
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	if (UseBlackboard(BBAsset, BlackboardPtr))
	{
		Blackboard->SetValueAsVector(BBKEY_HOMEPOS, GetPawn()->GetActorLocation());
		
		bool RunResult = RunBehaviorTree(BTAsset);
		
		ensure(RunResult);
	}	
}

void ACHAIControllerBase::StopAI()
{
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComponent)
	{
		ClearFocus(EAIFocusPriority::Gameplay);
		BTComponent->StopTree();  // 중지
	}
}

void ACHAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	RunAI();
}

void ACHAIControllerBase::HandleSightSense(AActor* Actor, FAIStimulus Stimulus)
{
	// Check if the stimulus is sight stimulus
	if (Stimulus.Type != UAISense::GetSenseID<UAISense_Sight>()) return;
}

/*void ACHAIController::HandleSoundSense(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.Type != UAISense::GetSenseID<UAISense_Hearing>()) return;

	// HandleSoundSense
}

void ACHAIController::HandleSenses(const TArray<AActor*>& Actors)
{	
	for (auto Target : Actors)
	{
		// if() 감지 여부
		if(CanSenseActor(Target))
		{
			HandleSensedSight(Target);			
		}
		else
		{
			HandleLostSight(Target);
		}
		// HandleSensedDamage(Target);

		
		/*FActorPerceptionBlueprintInfo Info;
		AIPerception->GetActorsPerception(Element,Info);
		for (auto Stimulus : Info.LastSensedStimuli)
		{
			
		}
		HandleSensedSound(AIPerception->GetActorsPerception(Element,Info));#1#
	}
}

void ACHAIController::HandleSensedSight(AActor* Actor)
{
	KnownSeenActors.AddUnique(Actor);
	switch (GetCurrentAIState())
	{
	case ECHAIState::Passive:
			SetStateAsAttacking(Actor,false);
		break;
	case ECHAIState::Attacking:
		break;
	case ECHAIState::Frozen:
			SetStateAsAttacking(Actor,false);
		break;
	case ECHAIState::Investigating:
			SetStateAsAttacking(Actor,false);
		break;
	case ECHAIState::Dead:
		break;
	case ECHAIState::Seeking:
			SetStateAsAttacking(Actor,false);
		break;
	default: ;
	}
}

void ACHAIController::HandleLostSight(AActor* Actor)
{
	if(Actor == AttackTarget)
	{
		switch (GetCurrentAIState())
		{
		case ECHAIState::Passive:
			break;
		case ECHAIState::Attacking:
			SetStateAsSeeking(Actor->GetActorLocation());
			break;
		case ECHAIState::Frozen:
			SetStateAsSeeking(Actor->GetActorLocation());
			break;
		case ECHAIState::Investigating:
			SetStateAsSeeking(Actor->GetActorLocation());
			break;
		case ECHAIState::Dead:
			break;
		case ECHAIState::Seeking:
			break;
		default: ;
		}
	}
}

void ACHAIController::HandleSensedSound(FVector Location)
{
	switch (GetCurrentAIState())
	{
	case ECHAIState::Passive:
			SetStateAsInvestigating(Location);
		break;
	case ECHAIState::Attacking:
		break;
	case ECHAIState::Frozen:
		break;
	case ECHAIState::Investigating:
			SetStateAsInvestigating(Location);
		break;
	case ECHAIState::Dead:
		break;
	case ECHAIState::Seeking:
			SetStateAsInvestigating(Location);
		break;
	default: ;
	}
}

void ACHAIController::HandleSensedDamage(AActor* Actor)
{
	switch (GetCurrentAIState())
	{
	case ECHAIState::Passive:
			SetStateAsAttacking(Actor,false);
		break;
	case ECHAIState::Attacking:
		break;
	case ECHAIState::Frozen:
		break;
	case ECHAIState::Investigating:
			SetStateAsAttacking(Actor,false);
		break;
	case ECHAIState::Dead:
		break;
	case ECHAIState::Seeking:
			SetStateAsAttacking(Actor,false);
		break;
	default: ;
	}
}

ECHAIState ACHAIController::GetCurrentAIState()
{
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	// uint8 num = BlackboardPtr->GetValueAsEnum(StateKeyName);
	CurrentAIState = static_cast<ECHAIState>(BlackboardPtr->GetValueAsEnum(StateKeyName));
	return CurrentAIState;
}

void ACHAIController::SetStateAsPassive()
{
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	BlackboardPtr->SetValueAsInt(StateKeyName,static_cast<int32>(ECHAIState::Passive));
}

void ACHAIController::SetStateAsAttacking(AActor* Target, bool bUseLastKnownAttackTarget)
{
	AActor* NewAttackTarget = Target;

	if(AttackTarget && bUseLastKnownAttackTarget)
	{
		NewAttackTarget = AttackTarget;
	}
	if(NewAttackTarget)
	{
		// NewAttackTarget 캐릭터 죽었는지 확인
		// if(NewAttackTarget.IsDead())
		// {
		//		SetStateAsPassive();
		// }
		UBlackboardComponent* BlackboardPtr = Blackboard.Get();
		BlackboardPtr->SetValueAsObject(AttackTargetKeyName, NewAttackTarget);
		BlackboardPtr->SetValueAsEnum(StateKeyName,static_cast<uint8>(ECHAIState::Attacking));
		BlackboardPtr->SetValueAsInt(StateKeyName,static_cast<int32>(ECHAIState::Attacking));
		AttackTarget = NewAttackTarget;
	}
	else
	{
		SetStateAsPassive();
	}
}

void ACHAIController::SetStateAsFrozen()
{
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	BlackboardPtr->SetValueAsInt(StateKeyName,static_cast<int32>(ECHAIState::Frozen));
}

void ACHAIController::SetStateAsInvestigating(FVector Location)
{
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	BlackboardPtr->SetValueAsInt(StateKeyName,static_cast<int32>(ECHAIState::Investigating));
	BlackboardPtr->SetValueAsVector(PointOfInterestKeyName, Location);
}

void ACHAIController::SetStateAsDead()
{
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	BlackboardPtr->SetValueAsInt(StateKeyName,static_cast<int32>(ECHAIState::Dead));
}

void ACHAIController::SetStateAsSeeking(FVector Location)
{
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	BlackboardPtr->SetValueAsInt(StateKeyName,static_cast<int32>(ECHAIState::Seeking));
	BlackboardPtr->SetValueAsVector(PointOfInterestKeyName, Location);	
}

bool ACHAIController::CanSenseActor(AActor* Target)
{
	FActorPerceptionBlueprintInfo Info;
	AIPerception->GetActorsPerception(Target,Info);
	
	for (auto Stimulus : Info.LastSensedStimuli)
	{		
		if (Stimulus.Type != UAISense::GetSenseID<UAISense_Sight>())
		{
			return Stimulus.WasSuccessfullySensed();			
		}
	}
	return false;
}*/

