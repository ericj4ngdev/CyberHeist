// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CHAI.h"
#include "Perception/AIPerceptionTypes.h"
#include "CHPathFollowingComponent.h"
#include "CHAIControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API ACHAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
	ACHAIControllerBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void RunAI();
	virtual void StopAI();
	
protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBlackboardData> BBAsset;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBehaviorTree> BTAsset;

	virtual void OnPossess(APawn* InPawn) override;


public:
	/*TArray<AActor*> KnownSeenActors;
	AActor* AttackTarget;

	UPROPERTY(EditAnywhere)
	FName StateKeyName;
	
	UPROPERTY(EditAnywhere)
	FName PointOfInterestKeyName;

	UPROPERTY(EditAnywhere)
	FName AttackTargetKeyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECHAIState CurrentAIState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECHAIMovementSpeed CurrentAIMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECHAISense AISense;*/
	/*UFUNCTION()
	void HandleSoundSense(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void HandleSenses(const TArray<AActor*>& Actors);

	UFUNCTION()
	void HandleSensedSight(AActor* Actor);

	UFUNCTION()
	void HandleLostSight(AActor* Actor);

	UFUNCTION()
	void HandleSensedSound(FVector Location);
	
	UFUNCTION()
	void HandleSensedDamage(AActor* Actor);

public:
	ECHAIState GetCurrentAIState();
	void SetStateAsPassive();
	void SetStateAsAttacking(AActor* Target, bool UseLastKnownAttackTarget);
	void SetStateAsFrozen();
	void SetStateAsInvestigating(FVector Location);
	void SetStateAsDead();
	void SetStateAsSeeking(FVector Location);
	bool CanSenseActor(AActor* Target);*/
};
