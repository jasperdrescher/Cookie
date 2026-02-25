// Copyright (c) 2026 Jasper Drescher. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "CkCombatPlayerOverheadWidget.generated.h"

UCLASS()
class COOKIE_API UCkCombatPlayerOverheadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Nameplate")
	void SetPlayerName(const FText& InName);

	UFUNCTION(BlueprintImplementableEvent, Category = "Nameplate")
	void SetNameColor(const FLinearColor& InColor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Life Bar")
	void SetLifePercentage(float Percent);

	UFUNCTION(BlueprintImplementableEvent, Category = "Life Bar")
	void SetBarColor(const FLinearColor& Color);
};
