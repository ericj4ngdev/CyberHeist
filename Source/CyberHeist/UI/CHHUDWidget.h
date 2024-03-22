// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
// #include "GameData/CHCharacterStat.h"
#include "CHHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UCHHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UCHHUDWidget(const FObjectInitializer& ObjectInitializer);

public:
	// void UpdateStat(const FCHCharacterStat& BaseStat, const FCHCharacterStat& ModifierStat);
	void SetMaxHp_Test(float NewMaxHp);
	void UpdateHpBar(float NewCurrentHp);

	void SetCombatMode(uint8 bCombat);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	TObjectPtr<class UCHHpBarWidget> HpBar;

	UPROPERTY()
	TObjectPtr<class UUCHCrossHairWidget> CrossHair;

	UPROPERTY()
	TObjectPtr<class UCHCharacterStatWidget> CharacterStat;
};
