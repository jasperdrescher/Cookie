// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#include "Player/CkPlayerControllerBase.h"

#include "Player/CkTextChatComponent.h"

void ACkPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	if (TextChatComponentClass && !TextChatComponent)
	{
		TextChatComponent = NewObject<UCkTextChatComponent>(this, TextChatComponentClass);

		if (TextChatComponent)
		{
			AddInstanceComponent(TextChatComponent);
			TextChatComponent->RegisterComponent();
		}
	}
}
