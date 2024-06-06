// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTS_StopAttackingIfTargetIsDead.h"
#include "CHAI.h"
#include "AIController.h"
#include "CHAIController.h"
#include "CyberHeist.h"
#include "EngineUtils.h"
#include "Interface/CHCharacterAIInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Character/CHCharacterBase.h"

UBTS_StopAttackingIfTargetIsDead::UBTS_StopAttackingIfTargetIsDead()
{
	bNotifyBecomeRelevant = true; // OnBecomeRelevant 호출을 위해 필요
}

void UBTS_StopAttackingIfTargetIsDead::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	// 파일 열 떄 호출. 실행해서 호출? 
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(__FUNCTION__))
	// 여기서 적들 가져온다.
	/*if (UWorld* World = GetWorld())
	{
		InitializeRecognizedPlayers(World);
	}*/
}

void UBTS_StopAttackingIfTargetIsDead::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(__FUNCTION__))
	
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn)
	{
		UWorld* World = ControllingPawn->GetWorld();
		if (World)
		{
			InitializeRecognizedPlayers(World);
		}
	}
}

void UBTS_StopAttackingIfTargetIsDead::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

	FindNearestAlivePlayer(OwnerComp);
	
}



void UBTS_StopAttackingIfTargetIsDead::InitializeRecognizedPlayers(UWorld* World)
{
	RecognizedPlayers.Empty();
	for (TActorIterator<ACHCharacterPlayer> It(World); It; ++It)
	{
		ACHCharacterPlayer* Character = *It;
		if (Character && !Character->GetIsDead())
		{
			RecognizedPlayers.Add(Character);
		}
	}
}

void UBTS_StopAttackingIfTargetIsDead::FindNearestAlivePlayer(UBehaviorTreeComponent& OwnerComp)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return;
	}

	APawn* ControllingPawn = AIController->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return;
	}
	
	float ClosestDistance = FLT_MAX;
	ACHCharacterPlayer* NearestPlayer = nullptr;

	for (ACHCharacterPlayer* Player : RecognizedPlayers)
	{
		// 1. 죽었는지 판단
		if (Player && !Player->GetIsDead()) 
		{
			// 2. 거리 계산
			float Distance = FVector::Dist(ControllingPawn->GetActorLocation(), Player->GetActorLocation());
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				NearestPlayer = Player;
			}
		}
	}

	// 거리 계산 끝나고 블랙보드 키값 정하기
	if (NearestPlayer)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(AttackTargetKey.SelectedKeyName, NearestPlayer);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(AttackTargetKey.SelectedKeyName);
	}
}
