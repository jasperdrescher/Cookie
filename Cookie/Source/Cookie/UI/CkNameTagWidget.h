// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "CkNameTagWidget.generated.h"

class UTextBlock;

UCLASS()
class COOKIE_API UCkNameTagWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Nameplate")
	void SetPlayerName(const FText& InName);

	UFUNCTION(BlueprintCallable, Category = "Nameplate")
	void SetNameColor(const FLinearColor& InColor);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerNameText;
};
