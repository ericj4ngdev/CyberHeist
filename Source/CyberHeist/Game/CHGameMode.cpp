// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/CHGameMode.h"

#include "CHGameState.h"
#include "CyberHeist.h"

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
		RestartGame();
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
	/*if (!HasMatchStarted())
	{
		// 캐릭터 생성 등의 로직
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			if (PC && PC->GetPawn() == nullptr)
			{
				RestartPlayer(PC);
			}
		}

		SetMatchState(MatchState::InProgress);
	}*/
	CH_LOG(LogCHNetwork, Log, TEXT("Match Started"))
}
