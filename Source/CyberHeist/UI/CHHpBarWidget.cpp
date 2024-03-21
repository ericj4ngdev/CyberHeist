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
	// �� ���� ������ UpdateHpBar�Լ� �����ؼ�
	// ������ ������Ʈ�� ������ �Լ�ȣ��Ǹ鼭 Hpbar ����
	ICHCharacterWidgetInterface* CharacterWidget = Cast<ICHCharacterWidgetInterface>(OwningActor);
	if (CharacterWidget) 
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
}

