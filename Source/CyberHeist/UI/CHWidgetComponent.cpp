// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CHWidgetComponent.h"
#include "UI/CHUserWidget.h"

void UCHWidgetComponent::InitWidget()
{
	Super::InitWidget();

	UCHUserWidget* CHUserWidget = Cast<UCHUserWidget>(GetWidget());
	if (CHUserWidget)
	{
		CHUserWidget->SetOwningActor(GetOwner());
	}
}