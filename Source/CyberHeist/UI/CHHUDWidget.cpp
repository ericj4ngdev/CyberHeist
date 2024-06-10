// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CHHUDWidget.h"
#include "Interface/CHCharacterHUDInterface.h"
#include "CHHpBarWidget.h"
#include "CHCharacterStatWidget.h"
#include "CyberHeist.h"
#include "UCHCrossHairWidget.h"

UCHHUDWidget::UCHHUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

//void UCHHUDWidget::UpdateStat(const FCHCharacterStat& BaseStat, const FCHCharacterStat& ModifierStat)
//{
//	FCHCharacterStat TotalStat = BaseStat + ModifierStat;
//	HpBar->SetMaxHp(TotalStat.MaxHp);
//
//	CharacterStat->UpdateStat(BaseStat, ModifierStat);
//}

void UCHHUDWidget::SetMaxHp_Test(float NewMaxHp)
{
	HpBar->SetMaxHp(NewMaxHp);
}

void UCHHUDWidget::UpdateHpBar(float NewCurrentHp)
{
	HpBar->UpdateHpBar(NewCurrentHp);
	// CrossHair
}

void UCHHUDWidget::SetCombatMode(uint8 bCombat)
{
	// HpBar->UpdateHpBar(NewCurrentHp);
	CrossHair->SetCombatMode(bCombat);
	
	bCombat ? CrossHair->SetVisibility(ESlateVisibility::Visible) : CrossHair->SetVisibility(ESlateVisibility::Hidden);
}


void UCHHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpBar = Cast<UCHHpBarWidget>(GetWidgetFromName(TEXT("WidgetHpBar")));
	ensure(HpBar);

	CrossHair = Cast<UUCHCrossHairWidget>(GetWidgetFromName(TEXT("WidgetCrossHair")));
	ensure(CrossHair);
	
	// CharacterStat = Cast<UCHCharacterStatWidget>(GetWidgetFromName(TEXT("WidgetCharacterStat")));
	// ensure(CharacterStat);
	UE_LOG(LogTemp, Log, TEXT("UCHHUDWidget::NativeConstruct()"))
	
	ICHCharacterHUDInterface* HUDPawn = Cast<ICHCharacterHUDInterface>(GetOwningPlayerPawn());
	if (HUDPawn)
	{
		// CH_LOG(LogCHNetwork, Log, TEXT("SetupHUDWidget"))
		UE_LOG(LogTemp, Log, TEXT("UCHHUDWidget::SetupHUDWidget"))
		HUDPawn->SetupHUDWidget(this);		
	}
}
