// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Character/CHCharacterNonPlayer.h"
#include "AI/CHAIControllerBase.h"
#include "Player/CHPlayerController.h"
#include "Weapon/Gun/CHGunBase.h"
#include "Spawner/CHWeaponSpawner.h"
#include "Spawner/CHSpawnTriggerArea.h"

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

	virtual void BeginPlay() override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	// virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;
	virtual void StartMatch() override;

public:
	// 모두 나가면 호출되는 함수
	void CleanUpLevel();

	// 재접속하면 호출될 함수
	void CustomResetLevel();

	UFUNCTION()
	void LoseCondition();
	
	UFUNCTION()
	void WinCondition();
	
	UPROPERTY(EditAnywhere)
	int32 MaxPlayers;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ACHCharacterNonPlayer*> CHAIPlayers;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ACHAIControllerBase*> CHAIControllers;

	// 무기
	UPROPERTY()
	TArray<ACHGunBase*> CHWeapons;

	// 무기 스포너 스폰 호출하기
	UPROPERTY()
	TArray<ACHWeaponSpawner*> CHWeaponSpawners;

	UPROPERTY()
	TArray<ACHSpawnTriggerArea*> CHSpawnTriggerAreas;
	
	
	
	// int32 NumPlayers;
};
