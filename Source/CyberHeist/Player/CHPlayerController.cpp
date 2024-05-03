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

void ACHPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// UE_LOG(LogTemp, Warning, TEXT("[ACHPlayerController::Tick] %s"),*PlayerCameraManager.GetName())
	// UE_LOG(LogTemp, Warning, TEXT("[ACHPlayerController::Tick] %s"),*PlayerCameraManager->GetCameraViewPoint())
	// GetPlayerViewPoint()
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

void ACHPlayerController::UpdateRotation(float DeltaTime)
{
	// FRotator DeltaRot(RotationInput);
	// FRotator ViewRotation = GetControlRotation();

	// UE_LOG(LogTemp, Warning, TEXT("[ACHPlayerController::UpdateRotation] DeltaRot: [%s], ViewRot: [%s]"), *DeltaRot.ToString(), *ViewRotation.ToString());
	
	Super::UpdateRotation(DeltaTime);
}
