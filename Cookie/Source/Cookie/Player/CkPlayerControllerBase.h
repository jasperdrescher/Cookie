// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "CkPlayerControllerBase.generated.h"

class UCkTextChatComponent;

UCLASS(abstract)
class COOKIE_API ACkPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TSubclassOf<UCkTextChatComponent> TextChatComponentClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCkTextChatComponent* TextChatComponent = nullptr;
};
