// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CombatDamageable.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "CombatDummy.generated.h"

class UStaticMeshComponent;
class UPhysicsConstraintComponent;

UCLASS(abstract)
class ACombatDummy : public AActor, public ICombatDamageable
{
	GENERATED_BODY()

public:
	ACombatDummy();

	virtual void ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse) override;
	virtual void HandleDeath() {}
	virtual void ApplyHealing(float Healing, AActor* Healer) {}
	virtual void NotifyDanger(const FVector& DangerLocation, AActor* DangerSource) override {}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BasePlate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Dummy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPhysicsConstraintComponent* PhysicsConstraint;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category="Combat", meta = (DisplayName = "On Dummy Damaged"))
	void BP_OnDummyDamaged(const FVector& Location, const FVector& Direction);
};
