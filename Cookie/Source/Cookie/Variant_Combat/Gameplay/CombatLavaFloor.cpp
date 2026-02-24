// Copyright (c) 2026 Jasper Drescher. All rights reserved.

#include "CombatLavaFloor.h"

#include "CombatDamageable.h"
#include "Components/StaticMeshComponent.h"

ACombatLavaFloor::ACombatLavaFloor()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	// create the mesh
	RootComponent = Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	// bind the hit handler
	Mesh->OnComponentHit.AddDynamic(this, &ACombatLavaFloor::OnFloorHit);
}

void ACombatLavaFloor::OnFloorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority())
		return;
	
	// check if the hit actor is damageable by casting to the interface
	if (ICombatDamageable* Damageable = Cast<ICombatDamageable>(OtherActor))
	{
		// damage the actor
		Damageable->ApplyDamage(Damage, this, Hit.ImpactPoint, FVector::ZeroVector);
	}
}
