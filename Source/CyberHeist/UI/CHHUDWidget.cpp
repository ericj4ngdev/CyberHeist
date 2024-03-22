// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CHHUDWidget.h"
#include "Interface/CHCharacterHUDInterface.h"
#include "CHHpBarWidget.h"
#include "CHCharacterStatWidget.h"

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
}

void UCHHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpBar = Cast<UCHHpBarWidget>(GetWidgetFromName(TEXT("WidgetHpBar")));
	ensure(HpBar);

	// CharacterStat = Cast<UCHCharacterStatWidget>(GetWidgetFromName(TEXT("WidgetCharacterStat")));
	// ensure(CharacterStat);

	// GetOwningPlayerPawn으로 컨트롤러가 빙의하고 있는 폰을 가져온다. 
	ICHCharacterHUDInterface* HUDPawn = Cast<ICHCharacterHUDInterface>(GetOwningPlayerPawn());
	if (HUDPawn)
	{
		HUDPawn->SetupHUDWidget(this);
	}
}
