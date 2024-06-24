// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_MoveGoals.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UEnvQueryContext_MoveGoals : public UEnvQueryContext
{
	GENERATED_BODY()

	
protected:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
