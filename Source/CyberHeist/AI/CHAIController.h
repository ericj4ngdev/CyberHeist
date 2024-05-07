// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CHAI.h"
#include "Perception/AIPerceptionTypes.h"
#include "CHAIController.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API ACHAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACHAIController();

public:
	void RunAI();
	void StopAI();

public:
	TArray<AActor*> KnownSeenActors;
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
	
protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBlackboardData> BBAsset;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBehaviorTree> BTAsset;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UAIPerceptionComponent> AIPerception;

	UFUNCTION()
	void HandleSightSense(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void HandleSoundSense(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void HandleSenses(const TArray<AActor*>& Actors);

	UFUNCTION()
	void HandleSensedSight(AActor* Actor);

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
	void CanSenseActor();
};
