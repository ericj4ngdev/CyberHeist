// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CHLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API ACHLobbyGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ACHLobbyGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxPlayers;

	UPROPERTY(EditAnywhere)
	FString ServerTravelPath;
};
