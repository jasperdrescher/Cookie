// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "CombatAIController.generated.h"

class UStateTreeAIComponent;

UCLASS(abstract)
class ACombatAIController : public AAIController
{
	GENERATED_BODY()

public:

	ACombatAIController();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStateTreeAIComponent* StateTreeAIComponent;
};
