// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CHCharacterBase.h"
#include "CHCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API ACHCharacterNonPlayer : public ACHCharacterBase
{
	GENERATED_BODY()
	
public:
	ACHCharacterNonPlayer();

public:
	virtual void SetDead() override;
};
