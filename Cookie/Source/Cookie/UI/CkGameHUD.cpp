// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#include "UI/CkGameHUD.h"

#include "UI/CkHUDWidget.h"

void ACkGameHUD::SetCookieCount(int32 NewCount)
{
	HUDWidget->SetCookieCount(NewCount);
}

void ACkGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UCkHUDWidget>(GetWorld()->GetFirstPlayerController(), HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}
}
