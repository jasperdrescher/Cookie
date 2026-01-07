// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#include "Actors/CkCookiePickup.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"

ACkCookiePickup::ACkCookiePickup()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(false);

	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRootComponent;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StaticMeshComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	StaticMeshComponent->SetGenerateOverlapEvents(true);
	StaticMeshComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	RotatingMovement->RotationRate = FRotator(SpinSpeedPitch, SpinSpeedYaw, SpinSpeedRoll);
	RotatingMovement->bUpdateOnlyIfRendered = true;
}

void ACkCookiePickup::BeginPlay()
{
	Super::BeginPlay();
}

void ACkCookiePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
