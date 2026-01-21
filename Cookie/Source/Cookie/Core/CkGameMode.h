// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "CkGameMode.generated.h"

UCLASS()
class COOKIE_API ACkGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	bool GetRoleForJoiningPlayer(APlayerController* NewPlayer) const;
};
