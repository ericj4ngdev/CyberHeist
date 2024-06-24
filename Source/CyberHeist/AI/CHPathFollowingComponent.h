// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "CHPathFollowingComponent.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UCHPathFollowingComponent : public UCrowdFollowingComponent
{
	GENERATED_BODY()

public:
	UCHPathFollowingComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual FAIRequestID RequestMove(const FAIMoveRequest& RequestData, FNavPathSharedPtr InPath) override;
	virtual void OnPathFinished(const FPathFollowingResult& Result) override;
};
