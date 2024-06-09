// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CHGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API ACHGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ACHGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void StartPlay() override;

	int32 MaxPlayers;

	// int32 NumPlayers;
};
