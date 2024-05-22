// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CHGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API ACHGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ACHGameMode();

	virtual void BeginPlay() override; 
	// GameModeBase.cpp 289줄
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage);

	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage);

	virtual void PostLogin(APlayerController* NewPlayer);

	// GameModeBase.cpp 155줄
	virtual void StartPlay();
};
