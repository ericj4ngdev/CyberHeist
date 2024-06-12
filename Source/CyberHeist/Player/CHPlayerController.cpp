// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CHPlayerController.h"
#include "UI/CHHUDWidget.h"
#include "CyberHeist.h"
#include "Character/CHCharacterPlayer.h"
#include "Game/CHGameMode.h"
#include "GameFramework/PawnMovementComponent.h"
#include "UI/CHResultWidget.h"

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
		
		CHResultWidget = CreateWidget<UCHResultWidget>(this, CHResultWidgetClass);
		if (CHResultWidget)
		{
			CHResultWidget->AddToViewport();
			CHResultWidget->SetVisibility(ESlateVisibility::Hidden);
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

void ACHPlayerController::ShowResult(uint8 bWin)
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"))

	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);
	
	FInputModeUIOnly InputModeData;
	SetInputMode(InputModeData);
	
	SetShowMouseCursor(true);

	// 현재 입력 상태를 초기화합니다.
	// 예: 현재 이동 중인 것을 멈추기 위해 캐릭터의 속도를 0으로 설정
	if (GetPawn())
	{
		ACHCharacterPlayer* CHPlayer = Cast<ACHCharacterPlayer>(GetPawn());
		if(CHPlayer)
		{
			// CHPlayer->GetCurrentWeapon()->DisableInput();
			CHPlayer->GetCurrentWeapon()->DisableWeaponInput();
			CHPlayer->GetMovementComponent()->StopMovementImmediately();			
		}
	}
	
	if(CHResultWidget)
	{
		// UI 표시 바꾸기
		CHResultWidget->SetVisibility(ESlateVisibility::Visible);	
		if(bWin)
		{
			CHResultWidget->ShowWinScreen();		
		}
		else
		{
			CHResultWidget->ShowLoseScreen();		
		}
	}
	
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"))
}

void ACHPlayerController::OnRestart()
{
	if (HasAuthority())
	{
		// If the controller is already on the server, reset the level directly
		ACHGameMode* CHGameMode = Cast<ACHGameMode>(GetWorld()->GetAuthGameMode());
		if (CHGameMode)
		{
			CHGameMode->ResetLevel();
		}
	}
	else
	{
		// If the controller is on the client, request the server to reset the level
		ServerRestartLevel();
	}
}

void ACHPlayerController::ServerRestartLevel_Implementation()
{
	ACHGameMode* CHGameMode = Cast<ACHGameMode>(GetWorld()->GetAuthGameMode());
	if (CHGameMode)
	{
		CHGameMode->ResetLevel();
		ACHCharacterPlayer* CHPlayer = Cast<ACHCharacterPlayer>(GetPawn());
		CHGameMode->RequestRespawn(CHPlayer,this);
	}
}

bool ACHPlayerController::ServerRestartLevel_Validate()
{
	return true;
}

void ACHPlayerController::SetPlayerInvincible(uint8 bPlayerInvincible)
{
	ACHCharacterPlayer* CHPlayer = Cast<ACHCharacterPlayer>(GetPawn());
	if(CHPlayer)
	{
		CHPlayer->SetInvincible(bPlayerInvincible);
	}
}

void ACHPlayerController::ClientShowResult_Implementation(uint8 bWin)
{
	ShowResult(bWin);
}
