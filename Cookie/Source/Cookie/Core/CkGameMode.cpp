// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#include "Core/CkGameMode.h"

#include "CookiePlayerController.h"
#include "Core/CkGamePlayerState.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"

void ACkGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ACkGamePlayerState* NewPlayerState = NewPlayer ? Cast<ACkGamePlayerState>(NewPlayer->PlayerState) : nullptr;
	const FString NewPlayerName = NewPlayerState ? NewPlayerState->GetPlayerName() : TEXT("Unknown");
	const bool bIsHost = GetRoleForJoiningPlayer(NewPlayer);
	const FString RoleText = bIsHost ? "Host" : "Client";

	if (GEngine)
	{
		const FString ConnectionMessage = FString::Printf(TEXT("%s (%s) connected"), *NewPlayerName, *RoleText);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, ConnectionMessage);
	}

	if (ACookiePlayerController* NewPlayerController = Cast<ACookiePlayerController>(NewPlayer))
	{
		NewPlayerController->ClientPostLogin(NewPlayerName, RoleText);
	}

	NewPlayerState->bIsHost = bIsHost;
}

bool ACkGameMode::GetRoleForJoiningPlayer(APlayerController* NewPlayer) const
{
	const bool bIsListenServer = (GetNetMode() == NM_ListenServer);
	return bIsListenServer && NewPlayer->IsLocalController();
}
