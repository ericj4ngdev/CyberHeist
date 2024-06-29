// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CHPlayerController.h"
#include "UI/CHHUDWidget.h"
#include "CyberHeist.h"
#include "Character/CHCharacterPlayer.h"
#include "Game/CHGameMode.h"
#include "Game/CHGameState.h"
#include "Game/CHPlayerState.h"
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
	// GameOnlyInputMode.SetMouseLockMode(EMouseLockMode::DoNotLock);
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

		// 게임 스테이트 가져와서 갱신
		ACHGameState* CHGameState = Cast<ACHGameState>(GetWorld()->GetGameState());
		CHGameState->OnTotalKilledEnemyCountChangedDelegate.AddUObject(CHResultWidget, &UCHResultWidget::UpdateTotalKillCount);
		CHResultWidget->UpdateTotalKillCount(CHGameState->GetTotalKilledEnemyCount());
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
		if(CHPlayer && CHPlayer->GetCurrentWeapon())
		{
			// CHPlayer->GetCurrentWeapon()->DisableInput();
			CHPlayer->GetCurrentWeapon()->DisableWeaponInput();
			CHPlayer->GetMovementComponent()->StopMovementImmediately();			
		}
	}
	ACHPlayerState* CHPlayerState = GetPlayerState<ACHPlayerState>(); 
	if(CHResultWidget)
	{
		// UI 표시 바꾸기
		CHResultWidget->SetVisibility(ESlateVisibility::Visible);
		
		// 결과 정보 띄우기(공통)
		CH_LOG(LogCHNetwork, Log, TEXT("W : %d"), CHPlayerState->GetKilledEnemyCount())
		CHResultWidget->UpdateTotalKillCount(CHPlayerState->GetKilledEnemyCount());
		
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

void ACHPlayerController::SetResultScreen()
{
	CH_LOG(LogCHNetwork, Log, TEXT("Begin"))

	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);
		
	SetShowMouseCursor(false);
	
	FInputModeGameOnly GameOnlyInputMode;
	// GameOnlyInputMode.SetMouseLockMode(EMouseLockMode::DoNotLock);
	SetInputMode(GameOnlyInputMode);

	CHResultWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ACHPlayerController::OnRestart()
{
	if (HasAuthority())
	{
		// If the controller is already on the server, reset the level directly
		ACHGameMode* CHGameMode = Cast<ACHGameMode>(GetWorld()->GetAuthGameMode());
		
		ACHPlayerState* CHPlayerState = GetPlayerState<ACHPlayerState>();
		// CHPlayerState->ResetKilledEnemyCount();
		if (CHGameMode)
		{
			CHGameMode->ResetLevel();
			ACHCharacterPlayer* CHPlayer = Cast<ACHCharacterPlayer>(GetPawn());
			CHGameMode->RequestRestartGame(CHPlayer);
		}
	}
	else
	{		
		// If the controller is on the client, request the server to reset the level
		ServerRestartLevel();
	}
}

void ACHPlayerController::OnRespawn()
{
	if (HasAuthority())
	{
		// If the controller is already on the server, reset the level directly
		ACHGameMode* CHGameMode = Cast<ACHGameMode>(GetWorld()->GetAuthGameMode());
		if (CHGameMode)
		{
			ACHCharacterPlayer* CHPlayer = Cast<ACHCharacterPlayer>(GetPawn());
			CHGameMode->RequestRespawn(CHPlayer, this);
		}
	}
	else
	{		
		// If the controller is on the client, request the server to reset the level
		ServerRespawn();
	}
}

void ACHPlayerController::ServerRespawn_Implementation()
{
	ACHGameMode* CHGameMode = Cast<ACHGameMode>(GetWorld()->GetAuthGameMode());
	if (CHGameMode)
	{
		ACHCharacterPlayer* CHPlayer = Cast<ACHCharacterPlayer>(GetPawn());
		CHGameMode->RequestRespawn(CHPlayer, this);
	}
}

bool ACHPlayerController::ServerRespawn_Validate()
{
	return true;
}

void ACHPlayerController::ServerRestartLevel_Implementation()
{
	ACHGameMode* CHGameMode = Cast<ACHGameMode>(GetWorld()->GetAuthGameMode());
	if (CHGameMode)
	{
		CHGameMode->ResetLevel();
		ACHCharacterPlayer* CHPlayer = Cast<ACHCharacterPlayer>(GetPawn());
		CHGameMode->RequestRestartGame(CHPlayer);
	}
}

bool ACHPlayerController::ServerRestartLevel_Validate()
{
	return true;
}

void ACHPlayerController::ClientSetResultScreen_Implementation()
{
	SetResultScreen();
}

void ACHPlayerController::SetPlayerInvincible(uint8 bPlayerInvincible)
{
	ACHCharacterPlayer* CHPlayer = Cast<ACHCharacterPlayer>(GetPawn());
	if(CHPlayer)
	{
		CHPlayer->SetInvincible(bPlayerInvincible);
	}
}

void ACHPlayerController::ClientShowResult_Implementation(uint8 bWin, const FPlayStatistics& FinalPlayStatistics)
{
	ACHPlayerState* CHPlayerState = GetPlayerState<ACHPlayerState>();
	CHPlayerState->SetPlayStatistics(FinalPlayStatistics);
	ShowResult(bWin);
}
