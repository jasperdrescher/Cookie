// Copyright (c) 2026 Jasper Drescher. All rights reserved.

#include "UI/CkCombatHUD.h"

#include "UI/CkCombatHUDWidget.h"

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
