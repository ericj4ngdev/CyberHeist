// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/CHGameMode.h"

#include "CHGameState.h"
#include "CyberHeist.h"
#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "Character/CHCharacterNonPlayer.h"
#include "AI/CHAIControllerBase.h"
#include "Character/CHCharacterPlayer.h"
#include "GameFramework/PlayerStart.h"
#include "Player/CHPlayerController.h"
#include "Weapon/Gun/CHGunBase.h"
#include "Spawner/CHWeaponSpawner.h"
#include "Spawner/CHSpawnTriggerArea.h"


ACHGameMode::ACHGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), MaxPlayers(2)
{
	bDelayedStart = true;
	
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/Engine.Blueprint'/Game/CyberHeist/Blueprint/BP_CHCharacterPlayer.BP_CHCharacterPlayer_C'"));
	
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	// PlayerControllerClass
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/Engine.Blueprint'/Game/CyberHeist/Blueprint/BP_CHPlayerController.BP_CHPlayerController_C'"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}

	CurrentNum = 0;
	CurrentAIs = 0;
	GameStateClass = ACHGameState::StaticClass();
}

void ACHGameMode::BeginPlay()
{
	CH_LOG(LogCHNetwork, Warning, TEXT("Begin"))
	Super::BeginPlay();
	UWorld* World = GetWorld();
	for (TActorIterator<ACHWeaponSpawner> It(World); It; ++It)
	{
		ACHWeaponSpawner* CHWeaponSpawner = *It;
		if (CHWeaponSpawner)
		{
			CHWeaponSpawners.Add(CHWeaponSpawner);
		}
	}
	
	for (TActorIterator<ACHSpawnTriggerArea> It(World); It; ++It)
	{
		ACHSpawnTriggerArea* CHSpawnTriggerArea = *It;
		if (CHSpawnTriggerArea)
		{
			CHSpawnTriggerAreas.Add(CHSpawnTriggerArea);
		}
	}

	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		APlayerStart* ChPlayerStart = *It;
		if (ChPlayerStart)
		{
			ChPlayerStarts.Add(ChPlayerStart);
		}
	}

	for (TActorIterator<ACHEndPoint> It(World); It; ++It)
	{
		ACHEndPoint* CHEndPoint = *It;
		if (CHEndPoint)
		{
			CHEndPoints.Add(CHEndPoint);
		}
	}
	
	CH_LOG(LogCHNetwork, Warning, TEXT("End"))
}

void ACHGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"))
	Super::InitGame(MapName, Options, ErrorMessage);

	if (IsRunningDedicatedServer())
	{
		// 초기화 로직
		CH_LOG(LogCHNetwork, Log, TEXT("GRPC_UpdateAddressToMatchmaker"))
	}
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"))
}

void ACHGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"))
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"))	
}

void ACHGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("============================================================"));
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"))
	// 2명 넘으면 접속 거부
	if(NumPlayers < MaxPlayers)
	{
		Super::PreLogin(Options, Address, UniqueId, ErrorMessage);		
	}
	else
	{
		ErrorMessage = TEXT("[FAIL] Session already full.");
		FGameModeEvents::GameModePreLoginEvent.Broadcast(this, UniqueId, ErrorMessage);
	}

	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"))
}

void ACHGameMode::PostLogin(APlayerController* NewPlayer)
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"))
	CH_LOG(LogCHNetwork, Log, TEXT("NumPlayers : %d | MaxPlayers : %d "), NumPlayers, MaxPlayers)
	Super::PostLogin(NewPlayer);
	CH_LOG(LogCHNetwork, Log, TEXT("NumPlayers : %d | MaxPlayers : %d "), NumPlayers, MaxPlayers)

	uint8 bMatchFull = NumPlayers == MaxPlayers;
	CH_LOG(LogCHNetwork, Log, TEXT("bMatchFull : %d "), bMatchFull)
	if(bMatchFull)
	{
		StartMatch();
		CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("StartMatch"))
	}
	
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"))
}

void ACHGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	bool bNobody = (NumPlayers <= 0);

	if (HasMatchStarted() && bNobody)
	{
		// 맵 계속 유지
		// 게임 초기화. 재입장 가능.
		// CleanUpLevel();
		// CustomResetLevel();
	}
}

void ACHGameMode::StartMatch()
{
	Super::StartMatch();
	CH_LOG(LogCHNetwork, Log, TEXT("Match Started"))
}

void ACHGameMode::ResetLevel()
{
	Super::ResetLevel();
	CleanUpLevel();
	CustomResetLevel();	
}

void ACHGameMode::SetPlayerDefaults(APawn* PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);

	ACHCharacterPlayer* CHPlayer = Cast<ACHCharacterPlayer>(PlayerPawn);
	CurrentNum++;
	CH_LOG(LogCHNetwork, Log, TEXT("CurrentNum : %d"), CurrentNum)
	// 플레이어 state로 비교 가능한가? 
	for (APlayerStart* Element : ChPlayerStarts)
	{
		int32 PlayerStartTagAsInt = FCString::Atoi(*Element->PlayerStartTag.ToString());
		if(PlayerStartTagAsInt == CurrentNum)
		{
			if(PlayerPawn)
			{
				CHPlayer->SetSpawnPoint(Element->GetTransform());
			}
		}
	}
	if(CurrentNum >= MaxPlayers) CurrentNum = 0;	// 초기화
}

void ACHGameMode::CleanUpLevel()
{
	CH_LOG(LogCHNetwork, Log, TEXT("Begin"))
	UWorld* World = GetWorld();
	for (TActorIterator<ACHCharacterNonPlayer> It(World); It; ++It)
	{
		ACHCharacterNonPlayer* CHAIPlayer = *It;
		if (CHAIPlayer)
		{
			CHAIPlayers.Add(CHAIPlayer);
		}
	}

	for (TActorIterator<ACHAIControllerBase> It(World); It; ++It)
	{
		ACHAIControllerBase* CHAIController = *It;
		if (CHAIController)
		{
			CHAIControllers.Add(CHAIController);
		}
	}
	// CurrentAIs = CHAIControllers.Num();
	// CH_LOG(LogCHNetwork, Log, TEXT("CHAIControllers : %d"), CHAIControllers.Num())
	
		
	for (TActorIterator<ACHGunBase> It(World); It; ++It)
	{
		ACHGunBase* CHWeapon = *It;
		if (CHWeapon)
		{
			CHWeapons.Add(CHWeapon);
		}
	}

	CH_LOG(LogCHNetwork, Log, TEXT("Before CHAIPlayers : %d"), CHAIPlayers.Num())
	CH_LOG(LogCHNetwork, Log, TEXT("Before CHAIControllers : %d"), CHAIControllers.Num())
	CH_LOG(LogCHNetwork, Log, TEXT("Before CHWeapons : %d"), CHWeapons.Num())
	
	for (ACHCharacterNonPlayer* Element : CHAIPlayers)
	{
		Element->Destroy();
	}
	for (ACHAIControllerBase* Element : CHAIControllers)
	{
		Element->Destroy();
	}
	for (ACHGunBase* Element : CHWeapons)
	{
		Element->Destroy();
	}
	
	CHAIPlayers.Empty();
	CHAIControllers.Empty();
	CHWeapons.Empty();

	CH_LOG(LogCHNetwork, Log, TEXT("After CHAIPlayers : %d"), CHAIPlayers.Num())
	CH_LOG(LogCHNetwork, Log, TEXT("After CHAIControllers : %d"), CHAIControllers.Num())
	CH_LOG(LogCHNetwork, Log, TEXT("After CHWeapons : %d"), CHWeapons.Num())
	
	CH_LOG(LogCHNetwork, Log, TEXT("End"))
}

void ACHGameMode::CustomResetLevel()
{
	CH_LOG(LogCHNetwork, Log, TEXT("Begin"))
	CH_LOG(LogCHNetwork, Log, TEXT("Before MatchState : %s"), *GetMatchState().ToString())
	SetMatchState(MatchState::EnteringMap);
	CH_LOG(LogCHNetwork, Log, TEXT("After MatchState : %s"), *GetMatchState().ToString())
	// 다시 스폰하게 만들기	
	for (ACHWeaponSpawner* Element : CHWeaponSpawners)
	{
		Element->SpawnGun();		
	}
	
	for (ACHSpawnTriggerArea* Element : CHSpawnTriggerAreas)
	{
		Element->Respawn();
		Element->BoxCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	for (ACHEndPoint* Element : CHEndPoints)
	{
		Element->BoxCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);;
	}
	CH_LOG(LogCHNetwork, Log, TEXT("End"))
}

void ACHGameMode::LoseCondition()
{
	CH_LOG(LogCHNetwork, Log, TEXT("Lose"))
	// 모든 PC에게 UI띄워주기
	for (FConstControllerIterator Iterator = GetWorld()->GetControllerIterator(); Iterator; ++Iterator)
	{
		AController* Controller = Iterator->Get();
		ACHPlayerController* PlayerController = Cast<ACHPlayerController>(Controller);
		if (PlayerController)
		{
			// 클라 RPC로 UI 활성화.
			PlayerController->ClientShowResult(false);
			PlayerController->SetPlayerInvincible(true);
		}
	}
}

void ACHGameMode::WinCondition()
{	
	CH_LOG(LogCHNetwork, Log, TEXT("Win"))
	for (FConstControllerIterator Iterator = GetWorld()->GetControllerIterator(); Iterator; ++Iterator)
	{
		AController* Controller = Iterator->Get();
		ACHPlayerController* PlayerController = Cast<ACHPlayerController>(Controller);
		if (PlayerController)
		{
			PlayerController->ClientShowResult(true);
			PlayerController->SetPlayerInvincible(true);
		}
	}
}

void ACHGameMode::RequestRestartGame(ACharacter* ResetCharacter)
{
	CH_LOG(LogCHNetwork, Log, TEXT("Begin"))
	if (ResetCharacter)
	{
		ResetCharacter->Reset();
		ResetCharacter->Destroy();
		CH_LOG(LogCHNetwork, Log, TEXT("Destroy"))
	}
	/*if(ResetController)
	{
		// UI 초기화
		// ResetController->SetPlayerInvincible(true);
		// 안해줘도 이미 파괴해서 상관없을 듯. 
		CH_LOG(LogCHNetwork, Log, TEXT("MatchState : %s "), *GetMatchState().ToString())
		RestartPlayer(ResetController);	// 이거 호출하면 폰 스폰하고 나중에 SetPlayerDefaults 호출되면서 위치 자동 정렬		
	}*/
	
	for (FConstControllerIterator Iterator = GetWorld()->GetControllerIterator(); Iterator; ++Iterator)
	{
		AController* Controller = Iterator->Get();
		ACHPlayerController* PlayerController = Cast<ACHPlayerController>(Controller);
		if (PlayerController)
		{
			PlayerController->ClientSetResultScreen();
			RestartPlayer(PlayerController);
			// PlayerController->SetPlayerInvincible(false);
		}
	}
	
	CH_LOG(LogCHNetwork, Log, TEXT("End"))
}

void ACHGameMode::RequestRespawn(ACharacter* ResetCharacter, AController* ResetController)
{
	if (ResetCharacter)
	{
		ResetCharacter->Reset();
		ResetCharacter->Destroy();
		CH_LOG(LogCHNetwork, Log, TEXT("Destroy"))
	}
	if(ResetController)
	{
		// UI 초기화
		// ResetController->SetPlayerInvincible(true);
		// 안해줘도 이미 파괴해서 상관없을 듯. 
		CH_LOG(LogCHNetwork, Log, TEXT("MatchState : %s "), *GetMatchState().ToString())
		RestartPlayer(ResetController);	// 이거 호출하면 폰 스폰하고 나중에 SetPlayerDefaults 호출되면서 위치 자동 정렬		
	}
	CH_LOG(LogCHNetwork, Log, TEXT("End"))
}
