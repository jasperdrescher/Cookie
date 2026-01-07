// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#include "AI/CkCookieMonsterCharacter.h"

// Sets default values
ACkCookieMonsterCharacter::ACkCookieMonsterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ACkCookieMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACkCookieMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACkCookieMonsterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
