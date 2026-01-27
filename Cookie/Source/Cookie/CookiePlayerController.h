// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "CookiePlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;
class UCkTextChatComponent;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class ACookiePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Client, Reliable)
	void ClientPostLogin(const FString& NewPlayerName, const FString& RoleText);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TSubclassOf<UCkTextChatComponent> TextChatComponentClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCkTextChatComponent* TextChatComponent = nullptr;

protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;
};
