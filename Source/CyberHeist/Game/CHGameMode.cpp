// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/CHGameMode.h"

ACHGameMode::ACHGameMode()
{
	// DefaultPawnClass
	// static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/CyberHeist.CHCharacterPlayer"));
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
}
