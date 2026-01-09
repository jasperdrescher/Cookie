// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#include "UI/CkNameTagWidget.h"

#include "Components/TextBlock.h"

void UCkNameTagWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCkNameTagWidget::SetPlayerName(const FText& InName)
{
	if (PlayerNameText)
	{
		PlayerNameText->SetText(InName);
	}
}

void UCkNameTagWidget::SetNameColor(const FLinearColor& InColor)
{
	if (PlayerNameText)
	{
		PlayerNameText->SetColorAndOpacity(InColor);
	}
}
