// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#include "Player/CkTextChatComponent.h"

#include "CookiePlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

UCkTextChatComponent::UCkTextChatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCkTextChatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCkTextChatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCkTextChatComponent::SendMessage(const FText& Message)
{
	const APlayerState* PlayerState = Cast<ACookiePlayerController>(GetOwner())->PlayerState;
	const FString SenderName = PlayerState->GetPlayerName();
	const FUniqueNetIdRepl SenderUniqueNetId = PlayerState->GetUniqueId();

	Server_SendMessage(Message, SenderName, SenderUniqueNetId);
}

void UCkTextChatComponent::Server_SendMessage_Implementation(const FText& Message, const FString& SenderName, const FUniqueNetIdRepl& SenderUniqueNetId)
{
	AGameStateBase* GameState = Cast<AGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	const TArray<TObjectPtr<APlayerState>>& Players = GameState->PlayerArray;
	for (int32 i = 0; i < Players.Num(); ++i)
	{
		ACookiePlayerController* PlayerController = Cast<ACookiePlayerController>(Players[i]->GetPlayerController());
		if (PlayerController)
		{
			PlayerController->TextChatComponent->Client_RecieveMessage_Implementation(Message, SenderName, SenderUniqueNetId);
		}
	}
}

void UCkTextChatComponent::Client_RecieveMessage_Implementation(const FText& Message, const FString& SenderName, const FUniqueNetIdRepl& SenderUniqueNetId)
{
	FBPUniqueNetId SenderBPUniqueNetId;
	SenderBPUniqueNetId.SetUniqueNetId(SenderUniqueNetId.GetUniqueNetId());
	BP_ReceiveMessage(Message, SenderName, SenderBPUniqueNetId);
}
