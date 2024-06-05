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
	// Super::OnBecomeRelevant(OwnerComp, NodeMemory);

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
	
	UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	ACoverSystem* CoverSystem = ACoverSystem::GetCoverSystem(OwnerController);

	if (AIPawn != nullptr && MyBlackboard != nullptr && CoverSystem != nullptr)
	{
		if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Cover::StaticClass())
		{
			FCover Cover = MyBlackboard->GetValue<UBlackboardKeyType_Cover>(BlackboardKey.GetSelectedKeyID());
			if (Cover.IsValid())
			{
				AIPawn->StopAimWeapon();
				AIPawn->TakeCover(Cover);
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
