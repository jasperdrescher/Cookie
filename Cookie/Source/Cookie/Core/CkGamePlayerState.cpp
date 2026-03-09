// Copyright (c) 2026 Jasper Drescher. All rights reserved.

#include "Core/CkGamePlayerState.h"

#include "Cookie.h"
#include "Core/CkGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ACkGamePlayerState::ACkGamePlayerState()
{
	bReplicates = true;
}

void ACkGamePlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
		return;

	PickRandomPlayerColor();
}

void ACkGamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACkGamePlayerState, bIsHost);
	DOREPLIFETIME(ACkGamePlayerState, PlayerColor);
	DOREPLIFETIME(ACkGamePlayerState, Cookies);
}

void ACkGamePlayerState::AddCookie()
{
	if (!HasAuthority())
		return;

	Cookies += 1;
	OnCookiesChanged.Broadcast(Cookies);
}

void ACkGamePlayerState::OnRep_Cookies()
{
	OnCookiesChanged.Broadcast(Cookies);
}

void ACkGamePlayerState::PickRandomPlayerColor()
{
	if (PlayerColor != FColor::White)
		return;
	
	if (AvailablePlayerColors.IsEmpty())
	{
		UE_LOG(LogCookie, Warning, TEXT("Array is empty, cannot get random item."));
		return;
	}

	UGameInstance* GameInstanceBase = UGameplayStatics::GetGameInstance(GetWorld());
	if (GameInstanceBase)
	{
		UCkGameInstance* GameInstance = Cast<UCkGameInstance>(GameInstanceBase);
		if (GameInstance)
		{
			const FUniqueNetIdRepl& UniqueNetIdRepl = GetUniqueId();
			TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId();
			if (FColor* Color = GameInstance->PlayerData.Find(UniqueNetId))
			{
				PlayerColor = *Color;
				return;
			}
		}
	}

	FRandomStream RandomStream;
	RandomStream.Initialize(FDateTime::Now().GetTicks());
	const int32 RandomIndex = RandomStream.RandRange(0, AvailablePlayerColors.Num() - 1);
	PlayerColor = AvailablePlayerColors[RandomIndex];

	if (GameInstanceBase)
	{
		UCkGameInstance* GameInstance = Cast<UCkGameInstance>(GameInstanceBase);
		if (GameInstance)
		{
			const FUniqueNetIdRepl& UniqueNetIdRepl = GetUniqueId();
			TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId();
			if (UniqueNetId.IsValid())
			{
				GameInstance->PlayerData.Add(UniqueNetId, PlayerColor);
			}
		}
	}
}
