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
	// Progress Bar 컨트롤을 위한 오브젝트 포인터
	UPROPERTY();
	TObjectPtr<class UProgressBar> HpProgressBar;

	UPROPERTY();
	float MaxHp;


};
