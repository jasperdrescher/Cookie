// Copyright (c) 2026 Jasper Drescher. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "CkCombatHUD.generated.h"

class UCkCombatHUDWidget;

UCLASS()
class COOKIE_API ACkCombatHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UCkCombatHUDWidget> HUDWidgetClass;

private:
	UPROPERTY()
	UCkCombatHUDWidget* HUDWidget = nullptr;
};
