// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AdvancedFriendsGameInstance.h"

#include "CkGameInstance.generated.h"

UCLASS()
class COOKIE_API UCkGameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	void SetSteamRichPresence(const FString& Key, const FString& Value);

	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	void ClearSteamRichPresence();
};
