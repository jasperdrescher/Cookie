// Copyright (c) 2026 Jasper Drescher. All rights reserved.

#include "Player/CkPlayerControllerBase.h"

#include "Player/CkTextChatComponent.h"

void ACkPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	if (TextChatComponentClass)
	{
		TextChatComponent = NewObject<UCkTextChatComponent>(this, TextChatComponentClass);
		if (TextChatComponent)
		{
			AddInstanceComponent(TextChatComponent);
			TextChatComponent->OnComponentCreated();
			TextChatComponent->RegisterComponent();
		}
	}
}
