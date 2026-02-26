// Copyright (c) 2026 Jasper Drescher. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "CkCombatHUDWidget.generated.h"

UCLASS()
class COOKIE_API UCkCombatHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Life Bar")
	void SetLifePercentage(float Percent);

	UFUNCTION(BlueprintImplementableEvent, Category = "Life Bar")
	void SetBarColor(const FLinearColor& Color);
};
