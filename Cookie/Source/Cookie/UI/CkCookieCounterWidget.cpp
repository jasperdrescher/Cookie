// Copyright (c) 2026 Jasper Drescher. All rights reserved.

#include "UI/CkCookieCounterWidget.h"

#include "Components/TextBlock.h"

void UCkCookieCounterWidget::SetCookieCount(int32 NewCount)
{
	if (CookieText)
	{
		CookieText->SetText(FText::AsNumber(NewCount));
	}
}
