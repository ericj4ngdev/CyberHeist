// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/CHGameMode.h"

#include "CHGameState.h"
#include "CyberHeist.h"

ACHGameMode::ACHGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// DefaultPawnClass
	// static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/CyberHeist.CHCharacterPlayer"));

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


void ACHGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
                           FString& ErrorMessage)
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("============================================================"));
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"))
	
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"))
}

APlayerController* ACHGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal,
	const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"))

	APlayerController* NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"))

	return NewPlayerController;
}

void ACHGameMode::PostLogin(APlayerController* NewPlayer)
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"))
	
	Super::PostLogin(NewPlayer);

	if(bool bMatchFull = (MaxPlayers <= NumPlayers))
	{
		StartMatch();
	}

	/*UNetDriver* NetDriver = GetNetDriver();
	if (NetDriver)
	{
		if (NetDriver->ClientConnections.Num() == 0)
		{
			CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("No Client Connection"));
		}
		else
		{
			for (const auto& Connection : NetDriver->ClientConnections)
			{
				CH_LOG(LogCHNetwork, Log, TEXT("Client Connections : %s"), *Connection->GetName());
			}
		}		
	}
	else
	{
		CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("No NetDriver"));
	}*/
	
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"))
}

void ACHGameMode::StartPlay()
{
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("Begin"))
	
	Super::StartPlay();
	
	CH_LOG(LogCHNetwork, Log, TEXT("%s"), TEXT("End"))
}
