// Copyright (c) 2026 Jasper Drescher. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "CkHUDWidget.generated.h"

class UTextBlock;

UCLASS()
class COOKIE_API UCkHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Cookies")
	void SetCookieCount(int32 NewCount);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CookieText;
};
