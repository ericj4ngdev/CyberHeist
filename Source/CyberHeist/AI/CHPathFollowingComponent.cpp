// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CHPathFollowingComponent.h"
#include "CHAIControllerBase.h"
#include "CHCloneAIController.h"
#include "AIContextSubsystem.h"

UCHPathFollowingComponent::UCHPathFollowingComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

FAIRequestID UCHPathFollowingComponent::RequestMove(const FAIMoveRequest& RequestData, FNavPathSharedPtr InPath)
{
	const FAIRequestID RequestID = Super::RequestMove(RequestData, InPath);
	if (RequestID.IsValid())
	{
		const FVector Destination = RequestData.GetDestination();
		if (Destination != FAISystem::InvalidLocation)
		{
			if (ACHAIControllerBase* Controller = GetOwner<ACHAIControllerBase>())
			{
				UAIContextSubsystem* ContextSubsystem = UAIContextSubsystem::Get(Controller);
				if (ensure(ContextSubsystem))
				{
					// Notify global AI context system about new movement goal. Others AIs can fetch the destination from the centralized subsystem
					ContextSubsystem->RegisterPathingMoveGoal(Controller, Destination);
				}
			}
		}
	}
	return RequestID;
}

void UCHPathFollowingComponent::OnPathFinished(const FPathFollowingResult& Result)
{
	if (ACHAIControllerBase* Controller = GetOwner<ACHAIControllerBase>())
	{
		UAIContextSubsystem* ContextSubsystem = UAIContextSubsystem::Get(Controller);
		if (ensure(ContextSubsystem))
		{
			// Movement request was finished -> clear goal location of this AI
			ContextSubsystem->UnregisterPathingMoveGoal(Controller);
		}
	}

	Super::OnPathFinished(Result);
}
