// Copyright (c) 2026 Jasper Drescher. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlueprintDataDefinitions.h"

#include "CkTextChatComponent.generated.h"

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COOKIE_API UCkTextChatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCkTextChatComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SendMessage(const FString& Message);

	UFUNCTION(BlueprintCallable)
	void SendAnnouncement(const FString& Message);

	UFUNCTION(Server, Reliable)
	void Server_SendMessage(const FString& Message);

	UFUNCTION(Server, Reliable)
	void Server_SendAnnouncement(const FString& Message);

	UFUNCTION(Client, Reliable)
	void Client_ReceiveMessage(const FString& Message, const FString& SenderName, const FUniqueNetIdRepl& SenderUniqueNetId);

	UFUNCTION(Client, Reliable)
	void Client_ReceiveAnnouncement(const FString& Message);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ReceiveMessage(const FString& Message, const FString& SenderName, const FBPUniqueNetId& SenderUniqueNetId);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ReceiveAnnouncement(const FString& Message);
};
