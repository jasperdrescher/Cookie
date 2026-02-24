// Copyright (c) 2026 Jasper Drescher. All rights reserved.

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

void ACombatDummy::ApplyDamage(float /*Damage*/, AActor* /*DamageCauser*/, const FVector& DamageLocation, const FVector& DamageImpulse)
{
	if (!HasAuthority())
		return;

	Dummy->AddImpulseAtLocation(DamageImpulse, DamageLocation);

	Multicast_ApplyDamage(DamageImpulse, DamageLocation);
}

void ACombatDummy::Multicast_ApplyDamage_Implementation(const FVector_NetQuantize& DamageImpulse, const FVector_NetQuantize& DamageLocation)
{
	BP_OnDummyDamaged(DamageLocation, DamageImpulse.GetSafeNormal());
}
