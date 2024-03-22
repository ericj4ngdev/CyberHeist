// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/CHUserWidget.h"
#include "UCHCrossHairWidget.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UUCHCrossHairWidget : public UCHUserWidget
{
	GENERATED_BODY()
	
public:
	UUCHCrossHairWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
public:
	void SetCombatMode(uint8 bCombat);
};
