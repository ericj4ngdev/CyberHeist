// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CHUserWidget.h"
#include "CHHpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class CYBERHEIST_API UCHHpBarWidget : public UCHUserWidget
{
	GENERATED_BODY()
	
public:
	UCHHpBarWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	FORCEINLINE void SetMaxHp(float NewMaxHp) { MaxHp = NewMaxHp; }
	void UpdateHpBar(float NewCurrentHp);

protected:
	// Progress Bar ��Ʈ���� ���� ������Ʈ ������
	UPROPERTY();
	TObjectPtr<class UProgressBar> HpProgressBar;

	UPROPERTY();
	float MaxHp;


};
