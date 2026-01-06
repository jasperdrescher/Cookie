// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CkCookiePickup.generated.h"

UCLASS()
class COOKIE_API ACkCookiePickup : public AActor
{
	GENERATED_BODY()
	
public:	
	ACkCookiePickup();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneRootComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
};
