// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Character/CHCharacterBase.h"
#include "Character/CHCharacterPlayer.h"
#include "BTS_StopAttackingIfTargetIsDead.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UBTS_StopAttackingIfTargetIsDead : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTS_StopAttackingIfTargetIsDead();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector AttackTargetKey;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ACHCharacterPlayer*> RecognizedPlayers;
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
private:
	void InitializeRecognizedPlayers(UWorld* World);
	void FindNearestAlivePlayer(UBehaviorTreeComponent& OwnerComp);
};
