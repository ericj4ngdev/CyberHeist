// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_Detect.h"
#include "CHAI.h"
#include "AIController.h"
#include "Interface/CHCharacterAIInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "Character/CHCharacterPlayer.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Stop Fire If Target IsDead");	
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{	
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	// FVector Center = ControllingPawn->GetActorLocation();
	
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
	
	UObject* TargetActor = OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGETACTOR);
	ACHCharacterPlayer* Player = Cast<ACHCharacterPlayer>(TargetActor);
	
	if(Player && Player->GetIsDead())
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGETACTOR, nullptr);	
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGETACTOR, TargetActor);	
	}

	
	/*float DetectRadius = AIPawn->GetAIDetectRange();

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(Detect), false, ControllingPawn);
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);
	
	if (bResult)
	{
		for (auto const& OverlapResult : OverlapResults)
		{
			APawn* Pawn = Cast<APawn>(OverlapResult.GetActor());
			// 플레이어 컨트롤러라면 
			if (Pawn && Pawn->GetController()->IsPlayerController())
			{
				// Target 키값에 Player 할당. 
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGETACTOR, Pawn);
				// 감지된 곳에 녹색 구, 점, 선 생성
				DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);

				DrawDebugPoint(World, Pawn->GetActorLocation(), 10.0f, FColor::Green, false, 0.2f);
				DrawDebugLine(World, ControllingPawn->GetActorLocation(), Pawn->GetActorLocation(), FColor::Green, false, 0.27f);
				return;
			}
		}
	}*/
	// 플레이어를 못찾거나 없으면 Target은 null값. NPC위치에 빨간 구 
	// OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGETACTOR, nullptr);
	// DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
}