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

	void UpdateRotation(float DeltaTime) override;
};
