// Copyright (c) 2025 Jasper Drescher. All rights reserved.

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
	void SendMessage(const FText& Message);

	UFUNCTION(Server, Reliable)
	void Server_SendMessage(const FText& Message, const FString& SenderName, const FUniqueNetIdRepl& SenderUniqueNetId);

	UFUNCTION(Client, Reliable)
	void Client_RecieveMessage(const FText& Message, const FString& SenderName, const FUniqueNetIdRepl& SenderUniqueNetId);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ReceiveMessage(const FText& Message, const FString& SenderName, const FBPUniqueNetId& SenderUniqueNetId);
};
