// Copyright (c) 2026 Jasper Drescher. All rights reserved.

#include "Core/CkGamePlayerState.h"

#include "Cookie.h"
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

	const int32 RandomIndex = FMath::RandRange(0, AvailablePlayerColors.Num() - 1);
	PlayerColor = AvailablePlayerColors[RandomIndex];
}
