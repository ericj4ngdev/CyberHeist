// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTS_StayCovered.h"

#include "BlackboardKeyType_Cover.h"
#include "Interface/CHCharacterAIInterface.h"

#include "AI/Services/BTService_OccupyCover.h"
#include "GameFramework/Actor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "VisualLogger/VisualLogger.h"

UBTS_StayCovered::UBTS_StayCovered()
{
	NodeName = "Stay Covered";
}

void UBTS_StayCovered::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	AAIController* OwnerController = OwnerComp.GetAIOwner();

	// =====================================================
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return;
	}
	ICHCharacterAIInterface* AIPawn = Cast<ICHCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return;
	}	
	// =====================================================
	
	UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	ACoverSystem* CoverSystem = ACoverSystem::GetCoverSystem(OwnerController);

	if (AIPawn != nullptr && MyBlackboard != nullptr && CoverSystem != nullptr)
	{
		if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Cover::StaticClass())
		{
			FCover Cover = MyBlackboard->GetValue<UBlackboardKeyType_Cover>(BlackboardKey.GetSelectedKeyID());
			if (Cover.IsValid())
			{
				if(Cover.Data.bLeftCoverStanding)
				{
					// 왼쪽 커버 재생
					UE_LOG(LogTemp, Log, TEXT("bLeftCoverStanding"));
					AIPawn->StopAimWeapon();
					AIPawn->Cover(true, false);
					//StopAim
					return;
				}

				if(Cover.Data.bRightCoverStanding)
				{
					// 오른쪽 커버 재생
					UE_LOG(LogTemp, Log, TEXT("bRightCoverStanding"));
					AIPawn->StopAimWeapon();
					AIPawn->Cover(true, true);
					return;
				}
			
				if(Cover.Data.bLeftCoverCrouched)
				{
					AIPawn->StopAimWeapon();
					AIPawn->Cover(false, false);
					return;
				}
				if(Cover.Data.bRightCoverCrouched)
				{
					AIPawn->StopAimWeapon();
					AIPawn->Cover(false,true);
					return;
				}
				if(Cover.Data.bFrontCoverCrouched)
				{
					AIPawn->StopAimWeapon();
					AIPawn->Cover(false, true);
					return;
				}
			}			
		}
	}

	
}

void UBTS_StayCovered::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	AAIController* OwnerController = OwnerComp.GetAIOwner();
	
	// =====================================================
	APawn* ControllingPawn = OwnerController->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return;
	}
	ICHCharacterAIInterface* AIPawn = Cast<ICHCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return;
	}	
	// =====================================================

	ACoverSystem* CoverSystem = ACoverSystem::GetCoverSystem(OwnerController);
	
	if (AIPawn != nullptr && CoverSystem != nullptr)
	{
		AIPawn->UnCover();
	}
}
