// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CHPlayerController.h"
#include "UI/CHHUDWidget.h"
#include "CyberHeist.h"

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
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"))

	Super::BeginPlay();

	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"))
	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	/*if(IsLocalController())
	{
		CHHUDWidget = CreateWidget<UCHHUDWidget>(this, CHHUDWidgetClass);
		if (CHHUDWidget)
		{
			CHHUDWidget->AddToViewport();
		}
	}*/
}

void ACHPlayerController::PostInitializeComponents()
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"))
	Super::PostInitializeComponents();
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"))
}

void ACHPlayerController::PostNetInit()
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"))
	Super::PostNetInit();

	UNetDriver* NetDriver = GetNetDriver();
	if (NetDriver)
	{
		if(NetDriver->ServerConnection)
		{
			CH_LOG(LogCHNetwork, Log, TEXT("Server Connection : %s"), *NetDriver->ServerConnection->GetName());			
		}
	}
	else
	{
		CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("No NetDriver"));
	}
	
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"))
}

void ACHPlayerController::OnPossess(APawn* InPawn)
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"))
	Super::OnPossess(InPawn);	
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"))
}

void ACHPlayerController::CreateWidgetIfNeed()
{
	if(IsLocalController())
	{
		CHHUDWidget = CreateWidget<UCHHUDWidget>(this, CHHUDWidgetClass);
		if (CHHUDWidget)
		{
			CHHUDWidget->AddToViewport();
		}
	}
}

void ACHPlayerController::ReturnToMainMenu()
{
	// Ensure this runs only on the client
	if (IsLocalController())
	{
		// Disconnect from the server
		GetWorld()->GetFirstPlayerController()->ClientTravel("/Game/CyberHeist/Maps/TitleScreen.TitleScreen", ETravelType::TRAVEL_Absolute);
	}
}
