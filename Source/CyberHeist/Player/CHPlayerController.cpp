// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CHPlayerController.h"
#include "UI/CHHUDWidget.h"

ACHPlayerController::ACHPlayerController()
{
	static ConstructorHelpers::FClassFinder<UCHHUDWidget> CHHUDWidgetRef(TEXT("/Game/CyberHeist/UI/WBP_CHHUD.WBP_CHHUD_C"));
	if (CHHUDWidgetRef.Class)
	{
		CHHUDWidgetClass = CHHUDWidgetRef.Class;
	}
}

void ACHPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	CHHUDWidget = CreateWidget<UCHHUDWidget>(this, CHHUDWidgetClass);
	if (CHHUDWidget)
	{
		CHHUDWidget->AddToViewport();
	}
}
