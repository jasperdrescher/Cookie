// Copyright (c) 2026 Jasper Drescher. All rights reserved.

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
	TMap<TSharedPtr<const FUniqueNetId>, FColor> PlayerData;
};
