// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_UnCoverAim.h"

#include "AIController.h"
#include "BlackboardKeyType_Cover.h"
#include "CoverSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/CHCharacterAIInterface.h"

#include "GameFramework/Actor.h"
#include "VisualLogger/VisualLogger.h"

UBTT_UnCoverAim::UBTT_UnCoverAim()
{
	NodeName = "BTT_UnCoverAim";
	const FString FilterName = GET_MEMBER_NAME_CHECKED(UBTT_UnCoverAim, CoverKey).ToString() + TEXT("_Cover");
	CoverKey.AllowedTypes.Add(NewObject<UBlackboardKeyType_Cover>(this, *FilterName));
}

EBTNodeResult::Type UBTT_UnCoverAim::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* OwnerController = OwnerComp.GetAIOwner();
	APawn* ControllingPawn = Cast<APawn>(OwnerController->GetPawn());
	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}
		
	ICHCharacterAIInterface* AIPawn = Cast<ICHCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	ACoverSystem* CoverSystem = ACoverSystem::GetCoverSystem(OwnerController);
	if (BlackboardComp != nullptr && CoverSystem != nullptr)
	{
		UE_LOG(LogTemp,Log,TEXT("[UBTT_UnCoverAim::ExecuteTask] 1"));
		if (CoverKey.SelectedKeyType == UBlackboardKeyType_Cover::StaticClass())
		{
			FCover Cover = BlackboardComp->GetValue<UBlackboardKeyType_Cover>(CoverKey.GetSelectedKeyID());
			if(Cover.IsValid())
			{
				UE_LOG(LogTemp,Log,TEXT("[UBTT_UnCoverAim::ExecuteTask] 2"));
				AIPawn->UnCoverAim(Cover);
				return EBTNodeResult::Succeeded;
			}
		}
	}	
	
	return EBTNodeResult::Failed;
}
