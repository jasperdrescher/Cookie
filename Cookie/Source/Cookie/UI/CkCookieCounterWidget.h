// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "CkCookieCounterWidget.generated.h"

class UTextBlock;

UCLASS()
class COOKIE_API UCkCookieCounterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Cookies")
	void SetCookieCount(int32 NewCount);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CookieText;
};
