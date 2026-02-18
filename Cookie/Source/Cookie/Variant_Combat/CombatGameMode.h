// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "CombatGameMode.generated.h"

UCLASS(abstract)
class ACombatGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ACombatGameMode();

	void EnemyDied();

protected:
	virtual void BeginPlay() override;

private:
	void Victory();

	int TotalSpawnCount = 0;
};
