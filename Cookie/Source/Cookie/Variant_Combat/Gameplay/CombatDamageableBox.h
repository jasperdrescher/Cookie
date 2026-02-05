// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CombatDamageable.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "CombatDamageableBox.generated.h"

UCLASS(abstract)
class ACombatDamageableBox : public AActor, public ICombatDamageable
{
	GENERATED_BODY()

public:	
	ACombatDamageableBox();

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse) override;
	virtual void HandleDeath() override;
	virtual void ApplyHealing(float Healing, AActor* Healer) override {}
	virtual void NotifyDanger(const FVector& DangerLocation, AActor* DangerSource) override {}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

protected:
	UPROPERTY(EditAnywhere, Category="Damage")
	float CurrentHP = 3.0f;

	UPROPERTY(EditAnywhere, Category="Damage", meta = (ClampMin = 0, ClampMax = 10, Units = "s"))
	float DeathDelayTime = 6.0f;

	FTimerHandle DeathTimer;

	UFUNCTION(BlueprintImplementableEvent, Category="Damage")
	void OnBoxDamaged(const FVector& DamageLocation, const FVector& DamageImpulse);

	UFUNCTION(BlueprintImplementableEvent, Category="Damage")
	void OnBoxDestroyed();

	void RemoveFromLevel();
};
