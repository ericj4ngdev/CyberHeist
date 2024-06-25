// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CHResultWidget.h"

#include "CyberHeist.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

UCHResultWidget::UCHResultWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}


void UCHResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind buttons to functions
	if (BtnRestart)
	{
		BtnRestart->OnClicked.AddDynamic(this, &UCHResultWidget::OnRestartClicked);
	}

	if (BtnMainMenu)
	{
		BtnMainMenu->OnClicked.AddDynamic(this, &UCHResultWidget::OnMainMenuClicked);
	}
}

void UCHResultWidget::ShowLoseScreen()
{
	if (LoseBorder)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoseBorder. %s"), *FString(__FUNCTION__))
		LoseBorder->SetVisibility(ESlateVisibility::Visible);
	}
	if (WinBorder)
	{
		UE_LOG(LogTemp, Warning, TEXT("WinBorder. %s"), *FString(__FUNCTION__))
		WinBorder->SetVisibility(ESlateVisibility::Hidden);
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(__FUNCTION__))
}

void UCHResultWidget::ShowWinScreen()
{
	if (WinBorder)
	{	
		WinBorder->SetVisibility(ESlateVisibility::Visible);
	}
	if (LoseBorder)
	{
		LoseBorder->SetVisibility(ESlateVisibility::Hidden);
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(__FUNCTION__))
}

void UCHResultWidget::OnRestartClicked()
{
	// Handle restart logic here
	
}

void UCHResultWidget::OnMainMenuClicked()
{
	// Handle return to main menu logic here
}

void UCHResultWidget::UpdateTotalKillCount(int32 TotalKilledMonsterCount)
{
	TotalKillCountText->SetText(FText::FromString(FString::FromInt(TotalKilledMonsterCount)));
}
