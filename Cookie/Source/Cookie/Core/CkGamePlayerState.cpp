// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#include "Core/CkGamePlayerState.h"

#include "Net/UnrealNetwork.h"

void ACkGamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACkGamePlayerState, bIsHost);
}
