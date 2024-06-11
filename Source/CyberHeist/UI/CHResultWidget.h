// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CHResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UCHResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UCHResultWidget(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION()
	void ShowLoseScreen();
	UFUNCTION()
	void ShowWinScreen();
	UFUNCTION()
	void OnRestartClicked();
	UFUNCTION()
	void OnMainMenuClicked();
protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> ResultCanvas;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBorder> LoseBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBorder> WinBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> LoseText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> WinText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> BtnRestart;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> BtnMainMenu;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> RestartText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> MainMenuText;
};
