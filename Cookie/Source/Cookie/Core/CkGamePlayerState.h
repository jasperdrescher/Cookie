// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "CkGamePlayerState.generated.h"

UCLASS()
class COOKIE_API ACkGamePlayerState : public APlayerState
{
	GENERATED_BODY()
	

public:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Team/Role")
	bool bIsHost = false;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
