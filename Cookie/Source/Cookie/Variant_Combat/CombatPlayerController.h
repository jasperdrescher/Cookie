// Copyright (c) 2026 Jasper Drescher. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Player/CkPlayerControllerBase.h"

#include "CombatPlayerController.generated.h"

class UInputMappingContext;
class ACombatCharacter;
class UUserWidget;

UCLASS(abstract, Config="Game")
class ACombatPlayerController : public ACkPlayerControllerBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	UPROPERTY(EditAnywhere, Category="Respawn")
	TSubclassOf<ACombatCharacter> CharacterClass;

	FTransform RespawnTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> VictoryWidgetClass;

	UPROPERTY()
	UUserWidget* VictoryWidget;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

public:
	void SetRespawnTransform(const FTransform& NewRespawn);

	UFUNCTION(Client, Reliable)
	void Client_ShowVictoryWidget();

protected:
	UFUNCTION()
	void OnPawnDestroyed(AActor* DestroyedActor);
};
