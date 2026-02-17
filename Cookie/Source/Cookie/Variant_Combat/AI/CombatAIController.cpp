// Copyright Epic Games, Inc. All Rights Reserved.

#include "CombatAIController.h"

#include "Components/StateTreeAIComponent.h"

ACombatAIController::ACombatAIController()
{
	StateTreeAIComponent = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAIComponent"));
	check(StateTreeAIComponent);

	StateTreeAIComponent->SetStartLogicAutomatically(false);

	bStartAILogicOnPossess = false;
	bStopAILogicOnUnposses = false;
	bAttachToPawn = true;
}

void ACombatAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ACombatAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (HasAuthority())
	{
		StateTreeAIComponent->StartLogic();
	}
}

void ACombatAIController::OnUnPossess()
{
	Super::OnUnPossess();

	if (HasAuthority())
	{
		StateTreeAIComponent->StopLogic("OnUnPossess");
	}
}
