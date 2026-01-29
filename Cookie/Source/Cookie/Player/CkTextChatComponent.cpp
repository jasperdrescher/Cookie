// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#include "Player/CkTextChatComponent.h"

#include "Cookie.h"
#include "CookiePlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

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

void UCkTextChatComponent::Server_SendMessage_Implementation(const FString& Message)
{
	const AActor* Owner = GetOwner();
	const APlayerController* SenderPlayerController = Cast<APlayerController>(Owner);
	if (!SenderPlayerController)
		return;

	const APlayerState* SenderPlayerState = SenderPlayerController->PlayerState;
	if (!SenderPlayerState) 
		return;

	const FString SenderName = SenderPlayerState->GetPlayerName();
	const FUniqueNetIdRepl& SenderUniqueId = SenderPlayerState->GetUniqueId();

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
			ClientTextChatComponent->Client_ReceiveMessage(Message, SenderName, SenderUniqueId);
		}
	}
}

void UCkTextChatComponent::Client_ReceiveMessage_Implementation(const FString& Message, const FString& SenderName, const FUniqueNetIdRepl& SenderUniqueNetId)
{
	FBPUniqueNetId SenderBPUniqueNetId;
	SenderBPUniqueNetId.SetUniqueNetId(SenderUniqueNetId.GetUniqueNetId());
	BP_ReceiveMessage(Message, SenderName, SenderBPUniqueNetId);
}
