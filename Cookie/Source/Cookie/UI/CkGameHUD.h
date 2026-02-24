// Copyright (c) 2026 Jasper Drescher. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "CkGameHUD.generated.h"

class UCkHUDWidget;

UCLASS()
class COOKIE_API ACkGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	void SetCookieCount(int32 NewCount);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UCkHUDWidget> HUDWidgetClass;

private:
	UPROPERTY()
	UCkHUDWidget* HUDWidget = nullptr;
};
