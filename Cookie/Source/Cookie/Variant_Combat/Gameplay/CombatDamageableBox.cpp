// Copyright (c) 2026 Jasper Drescher. All rights reserved.

#include "CombatDamageableBox.h"

#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

ACombatDamageableBox::ACombatDamageableBox()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	RootComponent = Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	Mesh->SetCollisionProfileName(FName("BlockAllDynamic"));
	Mesh->SetSimulatePhysics(true);
	Mesh->bNavigationRelevant = false;
}

void ACombatDamageableBox::RemoveFromLevel()
{
	Destroy();
}

void ACombatDamageableBox::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(DeathTimer);
}

void ACombatDamageableBox::ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse)
{
	// only process damage if we still have HP
	if (CurrentHP > 0.0f)
	{
		// apply the damage
		CurrentHP -= Damage;

		// are we dead?
		if (CurrentHP <= 0.0f)
		{
			HandleDeath();
		}

		// apply a physics impulse to the box, ignoring its mass
		Mesh->AddImpulseAtLocation(DamageImpulse * Mesh->GetMass(), DamageLocation);

		// call the BP handler to play effects, etc.
		OnBoxDamaged(DamageLocation, DamageImpulse);
	}
}

void ACombatDamageableBox::HandleDeath()
{
	// change the collision object type to Visibility so we ignore most interactions but still retain physics collisions
	Mesh->SetCollisionObjectType(ECC_Visibility);

	// call the BP handler to play effects, etc.
	OnBoxDestroyed();

	// set up the death cleanup timer
	GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &ACombatDamageableBox::RemoveFromLevel, DeathDelayTime);
}
