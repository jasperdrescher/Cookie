// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "CombatPlayerController.generated.h"

class UInputMappingContext;
class ACombatCharacter;

UCLASS(abstract, Config="Game")
class ACombatPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	UPROPERTY(EditAnywhere, Category="Respawn")
	TSubclassOf<ACombatCharacter> CharacterClass;

	FTransform RespawnTransform;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

public:
	void SetRespawnTransform(const FTransform& NewRespawn);

protected:
	UFUNCTION()
	void OnPawnDestroyed(AActor* DestroyedActor);
};
