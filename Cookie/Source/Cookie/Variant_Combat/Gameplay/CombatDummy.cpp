// Copyright Epic Games, Inc. All Rights Reserved.

#include "CombatDummy.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

ACombatDummy::ACombatDummy()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	BasePlate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Plate"));
	BasePlate->SetupAttachment(RootComponent);

	Dummy = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Dummy"));
	Dummy->SetupAttachment(RootComponent);
	Dummy->SetSimulatePhysics(true);

	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("Physics Constraint"));
	PhysicsConstraint->SetupAttachment(RootComponent);
	PhysicsConstraint->SetConstrainedComponents(BasePlate, NAME_None, Dummy, NAME_None);
}

void ACombatDummy::ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse)
{
	Dummy->AddImpulseAtLocation(DamageImpulse, DamageLocation);

	BP_OnDummyDamaged(DamageLocation, DamageImpulse.GetSafeNormal());
}
