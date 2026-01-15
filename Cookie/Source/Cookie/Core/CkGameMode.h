// Fill out your copyright notice in the Description page of Project Settings.

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
	FString GetRoleTextForJoiningPlayer(APlayerController* NewPlayer) const;
};
