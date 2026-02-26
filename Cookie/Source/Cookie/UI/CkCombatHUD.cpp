// Copyright (c) 2026 Jasper Drescher. All rights reserved.

#include "UI/CkCombatHUD.h"

#include "UI/CkCombatHUDWidget.h"

void ACkCombatHUD::SetLifePercentage(float Percent)
{
	if (HUDWidget)
	{
		Cast<UCkCombatHUDWidget>(HUDWidget)->SetLifePercentage(Percent);
	}
}

void ACkCombatHUD::SetBarColor(const FLinearColor& Color)
{
	if (HUDWidget)
	{
		Cast<UCkCombatHUDWidget>(HUDWidget)->SetBarColor(Color);
	}
}

void ACkCombatHUD::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UCkCombatHUDWidget>(GetWorld()->GetFirstPlayerController(), HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}
}
