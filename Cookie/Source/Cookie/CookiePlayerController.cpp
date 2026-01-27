// Copyright Epic Games, Inc. All Rights Reserved.

#include "CookiePlayerController.h"

#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Player/CkTextChatComponent.h"

void ACookiePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (TextChatComponentClass && !TextChatComponent)
	{
		TextChatComponent = NewObject<UCkTextChatComponent>(this, TextChatComponentClass);

		if (TextChatComponent)
		{
			AddInstanceComponent(TextChatComponent);
			TextChatComponent->RegisterComponent();
		}
	}
}

void ACookiePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
	}
}

void ACookiePlayerController::ClientPostLogin_Implementation(const FString& NewPlayerName, const FString& RoleText)
{
	if (!IsLocalController())
		return;
	
	if (GEngine)
	{
		const FString ConnectionMessage = FString::Printf(TEXT("%s (%s) connected"), *NewPlayerName, *RoleText);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, ConnectionMessage);
	}
}
