// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTS_EnterCover.h"

UBTS_EnterCover::UBTS_EnterCover()
{
}

void UBTS_EnterCover::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}

void UBTS_EnterCover::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);


	
}
