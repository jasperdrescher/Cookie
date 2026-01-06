// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#include "Actors/CkCookiePickup.h"

ACkCookiePickup::ACkCookiePickup()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRootComponent;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);
}

void ACkCookiePickup::BeginPlay()
{
	Super::BeginPlay();
}

void ACkCookiePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
