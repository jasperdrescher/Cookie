// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#include "Core/CkGameMode.h"

#include "CookiePlayerController.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"

void ACkGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	const APlayerState* NewPlayerState = NewPlayer ? NewPlayer->PlayerState : nullptr;
	const FString NewPlayerName = NewPlayerState ? NewPlayerState->GetPlayerName() : TEXT("Unknown");
	const FString RoleText = GetRoleTextForJoiningPlayer(NewPlayer);

	if (GEngine)
	{
		const FString ConnectionMessage = FString::Printf(TEXT("%s (%s) connected"), *NewPlayerName, *RoleText);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, ConnectionMessage);
	}

	if (ACookiePlayerController* NewPlayerController = Cast<ACookiePlayerController>(NewPlayer))
	{
		NewPlayerController->ClientPostLogin(NewPlayerName, RoleText);
	}
}

FString ACkGameMode::GetRoleTextForJoiningPlayer(APlayerController* NewPlayer) const
{
	const bool bIsListenServer = (GetNetMode() == NM_ListenServer);
	const bool bIsHostPlayer = bIsListenServer && NewPlayer->IsLocalController();
	return bIsHostPlayer ? TEXT("Host") : TEXT("Client");
}
