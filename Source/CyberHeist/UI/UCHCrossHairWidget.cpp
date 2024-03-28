// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UCHCrossHairWidget.h"
#include "Character/CHCharacterPlayer.h"
#include "Interface/CHCrossHairWidgetInterface.h" 

UUCHCrossHairWidget::UUCHCrossHairWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UUCHCrossHairWidget::NativeConstruct()
{
	Super::NativeConstruct();
    
    ICHCrossHairWidgetInterface* CharacterWidget = Cast<ICHCrossHairWidgetInterface>(OwningActor);
    if (CharacterWidget)
    {
        CharacterWidget->SetupCrossWidget(this);
    }
}

void UUCHCrossHairWidget::SetCombatMode(uint8 bCombat)
{
    // UE_LOG(LogTemp, Log, TEXT("UUCHCrossHairWidget"));
    UE_LOG(LogTemp, Log, TEXT("bCombat : %u"), static_cast<uint32>(bCombat));
    // bCombat ? SetVisibility(ESlateVisibility::Visible) : SetVisibility(ESlateVisibility::Hidden);
}
