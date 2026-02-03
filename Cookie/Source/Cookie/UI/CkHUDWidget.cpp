// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#include "UI/CkHUDWidget.h"

#include "Components/TextBlock.h"

void UCkHUDWidget::SetCookieCount(int32 NewCount)
{
	if (CookieText)
	{
		CookieText->SetText(FText::AsNumber(NewCount));
	}
}
