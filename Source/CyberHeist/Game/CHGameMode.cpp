// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/CHGameMode.h"

#include "CHGameState.h"
#include "CyberHeist.h"
#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "Character/CHCharacterNonPlayer.h"
#include "AI/CHAIControllerBase.h"
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

	GameStateClass = ACHGameState::StaticClass();
}

void ACHGameMode::BeginPlay()
{
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


void ACHGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
                           FString& ErrorMessage)
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

/*APlayerController* ACHGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal,
	const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"))

	APlayerController* NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"))
	
	return NewPlayerController;
}*/

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
		// RestartGame();
		CleanUpLevel();
		CustomResetLevel();
		
		// InitGame();
		// 게임 초기화. 재입장 가능.
		// 맵 계속 유지
	}
	else
	{
		if (IsRunningDedicatedServer())
		{
			// GRPC_UpdateAddressToMatchmaker();
			CH_LOG(LogCHNetwork, Log, TEXT("GRPC_UpdateAddressToMatchmaker"))			
		}
	}
	
}

void ACHGameMode::StartMatch()
{
	Super::StartMatch();
	CH_LOG(LogCHNetwork, Log, TEXT("Match Started"))
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
	
	for (auto Element : CHAIPlayers)
	{
		Element->Destroy();
	}
	for (auto Element : CHAIControllers)
	{
		Element->Destroy();
	}
	for (auto Element : CHWeapons)
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
	for (auto Element : CHWeaponSpawners)
	{
		Element->SpawnGun();		
	}
	for (auto Element : CHSpawnTriggerAreas)
	{
		Element->BoxCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);;
	}
	CH_LOG(LogCHNetwork, Log, TEXT("End"))
}
