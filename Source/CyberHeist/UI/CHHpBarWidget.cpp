// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CHHpBarWidget.h"
#include "Components/ProgressBar.h"
#include "Interface/CHCharacterWidgetInterface.h" 

UCHHpBarWidget::UCHHpBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxHp = -1.0f;
}

void UCHHpBarWidget::UpdateHpBar(float NewCurrentHp)
{
	ensure(MaxHp > 0.0f);

	UE_LOG(LogTemp,Log, TEXT("%f"), NewCurrentHp);
	if (HpProgressBar)
	{
		HpProgressBar->SetPercent(NewCurrentHp / MaxHp);
	}
}

void UCHHpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHpBar")));
	ensure(HpProgressBar);

	// OwningActor
	// 이 액터 정보에 UpdateHpBar함수 전달해서
	// 스탯이 업데이트될 때마다 함수호출되면서 Hpbar 변경
	ICHCharacterWidgetInterface* CharacterWidget = Cast<ICHCharacterWidgetInterface>(OwningActor);
	if (CharacterWidget) 
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
}

