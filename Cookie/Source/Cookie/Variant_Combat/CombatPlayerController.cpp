// Copyright Epic Games, Inc. All Rights Reserved.

#include "Variant_Combat/CombatPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Variant_Combat/CombatCharacter.h"

void ACombatPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ACombatPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
	}
}

void ACombatPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!HasAuthority())
		return;
	
	InPawn->OnDestroyed.AddDynamic(this, &ACombatPlayerController::OnPawnDestroyed);
}

void ACombatPlayerController::SetRespawnTransform(const FTransform& NewRespawn)
{
	// save the new respawn transform
	RespawnTransform = NewRespawn;
}

void ACombatPlayerController::Client_ShowVictoryWidget_Implementation()
{
	if (VictoryWidgetClass != nullptr && VictoryWidget == nullptr)
	{
		VictoryWidget = CreateWidget<UUserWidget>(this, VictoryWidgetClass);

		if (VictoryWidget)
		{
			VictoryWidget->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly().SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock));
		}
	}
}

void ACombatPlayerController::OnPawnDestroyed(AActor* /*DestroyedActor*/)
{
	if (ACombatCharacter* RespawnedCharacter = GetWorld()->SpawnActor<ACombatCharacter>(CharacterClass, RespawnTransform))
	{
		Possess(RespawnedCharacter);
	}
}
