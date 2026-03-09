// Copyright (c) 2026 Jasper Drescher. All rights reserved.

#include "Player/CkTextChatComponent.h"

#include "Cookie.h"
#include "Core/CkGamePlayerState.h"
#include "GameFramework/GameStateBase.h"

UCkTextChatComponent::UCkTextChatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	SetNetAddressable();
}

void UCkTextChatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCkTextChatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCkTextChatComponent::SendMessage(const FString& Message)
{
	Server_SendMessage(Message);
}

void UCkTextChatComponent::SendAnnouncement(const FString& Message)
{
	Server_SendAnnouncement(Message);
}

void UCkTextChatComponent::Server_SendMessage_Implementation(const FString& Message)
{
	const AActor* Owner = GetOwner();
	const APlayerController* SenderPlayerController = Cast<APlayerController>(Owner);
	if (!SenderPlayerController)
		return;

	const APlayerState* SenderPlayerStateBase = SenderPlayerController->PlayerState;
	if (!SenderPlayerStateBase)
		return;

	const ACkGamePlayerState* SenderPlayerState = Cast<ACkGamePlayerState>(SenderPlayerStateBase);
	if (!SenderPlayerState)
		return;

	const FString SenderName = SenderPlayerState->GetPlayerName();

	const FUniqueNetIdRepl& SenderUniqueId = SenderPlayerState->GetUniqueId();

	const FColor SenderColor = SenderPlayerState->PlayerColor;

	if (!SenderUniqueId.IsValid())
	{
		UE_LOG(LogCookie, Warning, TEXT("UniqueId not valid yet for %s (server)"), *SenderName);
		return;
	}

	AGameStateBase* GameState = GetWorld()->GetGameState();
	if (!GameState)
		return;

	for (APlayerState* ClientPlayerState : GameState->PlayerArray)
	{
		if (!ClientPlayerState)
			continue;

		APlayerController* ClientPlayerController = Cast<APlayerController>(ClientPlayerState->GetPlayerController());
		if (!ClientPlayerController)
			continue;

		if (UCkTextChatComponent* ClientTextChatComponent = ClientPlayerController->FindComponentByClass<UCkTextChatComponent>())
		{
			ClientTextChatComponent->Client_ReceiveMessage(Message, SenderName, SenderUniqueId, SenderColor);
		}
	}
}

void UCkTextChatComponent::Server_SendAnnouncement_Implementation(const FString& Message)
{
	AGameStateBase* GameState = GetWorld()->GetGameState();
	if (!GameState)
		return;

	for (APlayerState* ClientPlayerState : GameState->PlayerArray)
	{
		if (!ClientPlayerState)
			continue;

		APlayerController* ClientPlayerController = Cast<APlayerController>(ClientPlayerState->GetPlayerController());
		if (!ClientPlayerController)
			continue;

		if (UCkTextChatComponent* ClientTextChatComponent = ClientPlayerController->FindComponentByClass<UCkTextChatComponent>())
		{
			ClientTextChatComponent->Client_ReceiveAnnouncement(Message);
		}
	}
}

void UCkTextChatComponent::Client_ReceiveMessage_Implementation(const FString& Message, const FString& SenderName, const FUniqueNetIdRepl& SenderUniqueNetId, const FColor& aSenderColor)
{
	FBPUniqueNetId SenderBPUniqueNetId;
	SenderBPUniqueNetId.SetUniqueNetId(SenderUniqueNetId.GetUniqueNetId());
	BP_ReceiveMessage(Message, SenderName, SenderBPUniqueNetId, aSenderColor);
}

void UCkTextChatComponent::Client_ReceiveAnnouncement_Implementation(const FString& Message)
{
	BP_ReceiveAnnouncement(Message);
}
