// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTS_StopAttackingIfTargetIsDead.h"
#include "CHAI.h"
#include "AIController.h"
#include "CHAIController.h"
#include "Interface/CHCharacterAIInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Character/CHCharacterBase.h"

UBTS_StopAttackingIfTargetIsDead::UBTS_StopAttackingIfTargetIsDead()
{
}

void UBTS_StopAttackingIfTargetIsDead::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return;
	}

	UWorld* World = ControllingPawn->GetWorld();
	if (nullptr == World)
	{
		return;
	}

	ICHCharacterAIInterface* AIPawn = Cast<ICHCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	
	UObject* Temp = BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName);
	AActor* Target = Cast<AActor>(Temp);
	/*AActor* Target = UBTFunctionLibrary::GetBlackboardValueAsActor(this,AttackTargetKey);*/
	ACHCharacterBase* TargetActor = Cast<ACHCharacterBase>(Target);

	if(TargetActor->GetIsDead())
	{
		Cast<ACHAIController>(AIOwner)->SetStateAsPassive();
	}
	
}
