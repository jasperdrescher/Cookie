// Copyright Epic Games, Inc. All Rights Reserved.

#include "Variant_Combat/CombatGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Variant_Combat/AI/CombatEnemySpawner.h"
#include "Variant_Combat/CombatPlayerController.h"

ACombatGameMode::ACombatGameMode()
{
}

void ACombatGameMode::BeginPlay()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACombatEnemySpawner::StaticClass(), FoundActors);

	UE_LOG(LogTemp, Log, TEXT("Found %i spawners"), FoundActors.Num());

	for (AActor* FoundActor : FoundActors)
	{
		if (ACombatEnemySpawner* CombatEnemySpawner = Cast< ACombatEnemySpawner>(FoundActor))
		{
			TotalSpawnCount += CombatEnemySpawner->GetSpawnCount();
		}
	}
}

void ACombatGameMode::Victory()
{
	if (const UWorld* World = GetWorld())
	{
		for (FConstPlayerControllerIterator PlayerControllerIterator = World->GetPlayerControllerIterator(); PlayerControllerIterator; ++PlayerControllerIterator)
		{
			APlayerController* PlayerController = PlayerControllerIterator->Get();
			if (PlayerController)
			{
				if (ACombatPlayerController* CombatPlayerController = Cast< ACombatPlayerController>(PlayerController))
				{
					CombatPlayerController->Client_ShowVictoryWidget();
				}
			}
		}
	}
}

void ACombatGameMode::EnemyDied()
{
	--TotalSpawnCount;

	if (TotalSpawnCount <= 0)
	{
		Victory();
	}
}
