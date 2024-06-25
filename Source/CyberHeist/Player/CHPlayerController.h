// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CHPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API ACHPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ACHPlayerController();

	virtual void Tick(float DeltaSeconds) override;
protected:
	virtual void BeginPlay() override;

// HUD Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class UCHHUDWidget> CHHUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class UCHHUDWidget> CHHUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class UCHResultWidget> CHResultWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class UCHResultWidget> CHResultWidget;

protected:
	virtual void PostInitializeComponents() override;
	virtual void PostNetInit() override;
	virtual void OnPossess(APawn* InPawn) override;
public:
	void CreateWidgetIfNeed();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ReturnToMainMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowResult(uint8 bWin);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetResultScreen();
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void OnRestart();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void OnRespawn();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRestartLevel();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRespawn();
	
	UFUNCTION(Client, Reliable)
	void ClientShowResult(uint8 bWin);

	UFUNCTION(Client, Reliable)
	void ClientSetResultScreen();
	

public:
	UFUNCTION()
	void SetPlayerInvincible(uint8 bPlayerInvincible);
};
