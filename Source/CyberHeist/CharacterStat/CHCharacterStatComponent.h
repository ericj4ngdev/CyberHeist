// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CHCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float /*Current HP*/);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CYBERHEIST_API UCHCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCHCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void InitializeComponent() override;

protected:
	void SetHp(float NewHp);

	UPROPERTY(VisibleInstanceOnly, Category = Stat)
		float MaxHp;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
		float CurrentHp;
public:
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;

	FORCEINLINE float GetMaxHp() { return MaxHp; }
	FORCEINLINE float GetCurrentHp() { return CurrentHp; }
	float ApplyDamage(float InDamage);

	// Combat
public:
	// FOnCombatModeSignature OnCombat;
};
