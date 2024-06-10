// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/CHLobbyGameMode.h"

#include "CyberHeist.h"
#include "GameFramework/GameState.h"

ACHLobbyGameMode::ACHLobbyGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), MaxPlayers(2)
{
	GameStateClass = AGameState::StaticClass();

	// bUseSeamlessTravel = true;
}

void ACHLobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	if (NumPlayers <= MaxPlayers)
	{
		Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	}
	else
	{
		ErrorMessage = TEXT("[FAIL] Session already full.");
		FGameModeEvents::GameModePreLoginEvent.Broadcast(this, UniqueId, ErrorMessage);
	}
}

void ACHLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"))
	Super::PostLogin(NewPlayer);

	int32 NumCurrentPlayers = GameState->PlayerArray.Num();
	
	CH_LOG(LogCHNetwork, Log, TEXT("NumCurrentPlayers : %d"), NumCurrentPlayers)
	
	if(NumCurrentPlayers >= MaxPlayers)
	{
		UWorld* World = GetWorld();
		check(World);

		// World->ServerTravel("127.0.0.1");
		CH_LOG(LogCHNetwork, Log, TEXT("%d"), NumCurrentPlayers)
		// World->ServerTravel(ServerTravelPath);	
	}
	
	/*UNetDriver* NetDriver = GetNetDriver();
	check(NetDriver);

	if (NetDriver->ClientConnections.Num() == 0)
	{
		CH_LOG(LogCHNetwork, Warning, TEXT("No Client Connection"));
	}

	for (const auto& Connection : NetDriver->ClientConnections)
	{
		CH_LOG(LogCHNetwork, Warning, TEXT("Client Connections: %s"), *Connection->GetName());
	}*/
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"))
}
