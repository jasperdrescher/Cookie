// Copyright Epic Games, Inc. All Rights Reserved.

#include "CombatEnemy.h"

#include "Animation/AnimInstance.h"
#include "CombatAIController.h"
#include "CombatLifeBar.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "MotionWarpingComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

ACombatEnemy::ACombatEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	// bind the attack montage ended delegate
	OnAttackMontageEnded.BindUObject(this, &ACombatEnemy::AttackMontageEnded);

	// set the AI Controller class by default
	AIControllerClass = ACombatAIController::StaticClass();

	// use an AI Controller regardless of whether we're placed or spawned
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// ignore the controller's yaw rotation
	bUseControllerRotationYaw = false;

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));

	// create the life bar
	LifeBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("LifeBar"));
	LifeBar->SetupAttachment(RootComponent);

	// set the collision capsule size
	GetCapsuleComponent()->SetCapsuleSize(35.0f, 90.0f);

	// set the character movement properties
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	// reset HP to maximum
	CurrentHP = MaxHP;

	Tags.Add(FName("Enemy"));
}

void ACombatEnemy::DoAIComboAttack()
{
	if (!HasAuthority())
		return;

	// ignore if we're already playing an attack animation
	if (bIsAttacking)
	{
		return;
	}

	// raise the attacking flag
	bIsAttacking = true;

	// choose how many times we're going to attack
	TargetComboCount = FMath::RandRange(1, ComboSectionNames.Num() - 1);

	// reset the attack counter
	CurrentComboAttack = 0;

	if (!ComboAttackMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Missing ComboAttackMontage"));
		return;
	}

	Server_PlayAnimMontage(ComboAttackMontage);

	const float MontageLength = PlayAnimMontage(EnemyPlayMontageInfo.Montage, EnemyPlayMontageInfo.PlayRate, EnemyPlayMontageInfo.StartSectionName);
	if (MontageLength > 0.f)
	{
		if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
		{
			AnimInst->Montage_SetEndDelegate(OnAttackMontageEnded, ComboAttackMontage);
		}
	}
}

void ACombatEnemy::DoAIChargedAttack()
{
	if (!HasAuthority())
		return;

	// ignore if we're already playing an attack animation
	if (bIsAttacking)
	{
		return;
	}

	// raise the attacking flag
	bIsAttacking = true;

	// choose how many loops are we going to charge for
	TargetChargeLoops = FMath::RandRange(MinChargeLoops, MaxChargeLoops);

	// reset the charge loop counter
	CurrentChargeLoop = 0;

	if (!ChargedAttackMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Missing ChargedAttackMontage"));
		return;
	}

	Server_PlayAnimMontage(ChargedAttackMontage);

	const float MontageLength = PlayAnimMontage(EnemyPlayMontageInfo.Montage, EnemyPlayMontageInfo.PlayRate, EnemyPlayMontageInfo.StartSectionName);
	if (MontageLength > 0.f)
	{
		if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
		{
			AnimInst->Montage_SetEndDelegate(OnAttackMontageEnded, ChargedAttackMontage);
		}
	}
}

void ACombatEnemy::AttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// reset the attacking flag
	bIsAttacking = false;

	// call the attack completed delegate so the StateTree can continue execution
	OnAttackCompleted.ExecuteIfBound();
}

const FVector& ACombatEnemy::GetLastDangerLocation() const
{
	return LastDangerLocation;
}

float ACombatEnemy::GetLastDangerTime() const
{
	return LastDangerTime;
}

void ACombatEnemy::DoAttackTrace(FName DamageSourceBone)
{
	if (!HasAuthority())
		return;

	// sweep for objects in front of the character to be hit by the attack
	TArray<FHitResult> OutHits;

	// start at the provided socket location, sweep forward
	const FVector TraceStart = GetMesh()->GetSocketLocation(DamageSourceBone);
	const FVector TraceEnd = TraceStart + (GetActorForwardVector() * MeleeTraceDistance);

	// enemies only affect Pawn collision objects; they don't knock back boxes
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	// use a sphere shape for the sweep
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(MeleeTraceRadius);

	// ignore self
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->SweepMultiByObjectType(OutHits, TraceStart, TraceEnd, FQuat::Identity, ObjectParams, CollisionShape, QueryParams))
	{
		// iterate over each object hit
		for (const FHitResult& CurrentHit : OutHits)
		{
			/** does the actor have the player tag? */
			if (CurrentHit.GetActor()->ActorHasTag(FName("Player")))
			{
				// check if the actor is damageable
				ICombatDamageable* Damageable = Cast<ICombatDamageable>(CurrentHit.GetActor());

				if (Damageable)
				{
					// knock upwards and away from the impact normal
					const FVector Impulse = (CurrentHit.ImpactNormal * -MeleeKnockbackImpulse) + (FVector::UpVector * MeleeLaunchImpulse);

					// pass the damage event to the actor
					Damageable->ApplyDamage(MeleeDamage, this, CurrentHit.ImpactPoint, Impulse);

				}
			}
		}
	}
}

void ACombatEnemy::CheckCombo()
{
	if (!HasAuthority())
		return;

	// increase the combo counter
	++CurrentComboAttack;

	// do we still have attacks to play in this string?
	if (CurrentComboAttack < TargetComboCount)
	{
		// jump to the next attack section
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			Multicast_PlayMontageSection(ComboSectionNames[CurrentComboAttack]);

			AnimInstance->Montage_JumpToSection(ComboSectionNames[CurrentComboAttack], ComboAttackMontage);
		}
	}
}

void ACombatEnemy::CheckChargedAttack()
{
	if (!HasAuthority())
		return;

	// increase the charge loop counter
	++CurrentChargeLoop;

	// jump to either the loop or attack section of the montage depending on whether we hit the loop target
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		Multicast_PlayMontageSection(CurrentChargeLoop >= TargetChargeLoops ? ChargeAttackSection : ChargeLoopSection);

		AnimInstance->Montage_JumpToSection(CurrentChargeLoop >= TargetChargeLoops ? ChargeAttackSection : ChargeLoopSection, ChargedAttackMontage);
	}
}

void ACombatEnemy::ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse)
{
	if (!HasAuthority())
		return;

	// pass the damage event to the actor
	FDamageEvent DamageEvent;
	const float ActualDamage = TakeDamage(Damage, DamageEvent, nullptr, DamageCauser);

	// only process knockback and effects if we received nonzero damage
	if (ActualDamage > 0.f)
	{
		Multicast_ApplyDamage(DamageLocation, DamageImpulse);

		Multicast_PlayDamageReceivedEffect(ActualDamage, DamageLocation, DamageImpulse.GetSafeNormal());
	}
}

void ACombatEnemy::Multicast_PlayDamageReceivedEffect_Implementation(float Damage, const FVector_NetQuantize& DamageLocation, const FVector_NetQuantize& DamageImpulse)
{
	ReceivedDamage(Damage, DamageLocation, DamageImpulse.GetSafeNormal());
}

void ACombatEnemy::Multicast_ApplyDamage_Implementation(const FVector_NetQuantize& DamageLocation, const FVector_NetQuantize& DamageImpulse)
{
	// apply the knockback impulse
	GetCharacterMovement()->AddImpulse(DamageImpulse, true);

	// is the character ragdolling?
	if (GetMesh()->IsSimulatingPhysics())
	{
		// apply an impulse to the ragdoll
		GetMesh()->AddImpulseAtLocation(DamageImpulse * GetMesh()->GetMass(), DamageLocation);
	}

	// stop the attack montages to interrupt the attack
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Stop(0.1f, ComboAttackMontage);
		AnimInstance->Montage_Stop(0.1f, ChargedAttackMontage);
	}
}

void ACombatEnemy::Multicast_HandleDamage_Implementation()
{
	GetMesh()->SetPhysicsBlendWeight(0.5f);
	GetMesh()->SetBodySimulatePhysics(PelvisBoneName, false);
}

void ACombatEnemy::Multicast_HandleDeath_Implementation()
{
	// hide the life bar
	LifeBar->SetHiddenInGame(true);

	// disable the collision capsule to avoid being hit again while dead
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// disable character movement
	GetCharacterMovement()->DisableMovement();

	// enable full ragdoll physics
	GetMesh()->SetSimulatePhysics(true);
}

void ACombatEnemy::Multicast_Landed_Implementation()
{
	GetMesh()->SetPhysicsBlendWeight(0.f);
}

void ACombatEnemy::HandleDeath()
{
	if (!HasAuthority())
		return;

	GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &ACombatEnemy::RemoveFromLevel, DeathRemovalTime);

	OnEnemyDied.Broadcast();

	Multicast_HandleDeath();
}

void ACombatEnemy::ApplyHealing(float Healing, AActor* Healer)
{
	// stub
}

void ACombatEnemy::NotifyDanger(const FVector& DangerLocation, AActor* DangerSource)
{
	if (!HasAuthority())
		return;

	// ensure we're being attacked by the player
	if (DangerSource && DangerSource->ActorHasTag(FName("Player")))
	{
		// save the danger location and game time
		LastDangerLocation = DangerLocation;
		LastDangerTime = GetWorld()->GetTimeSeconds();
	}
}

void ACombatEnemy::RemoveFromLevel()
{
	// destroy this actor
	Destroy();
}

float ACombatEnemy::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!HasAuthority())
		return 0.f;

	if (CurrentHP <= 0.f)
		return 0.f;

	CurrentHP -= Damage;

	UpdateLifeBar();

	if (CurrentHP <= 0.f)
	{
		HandleDeath();
	}
	else
	{
		Multicast_HandleDamage();
	}

	return Damage;
}

void ACombatEnemy::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	// is the character still alive?
	if (CurrentHP >= 0.f)
	{
		Multicast_Landed();
	}

	// call the landed Delegate for StateTree
	OnEnemyLanded.ExecuteIfBound();
}

void ACombatEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACombatEnemy, bIsAttacking);
	DOREPLIFETIME(ACombatEnemy, CurrentHP);
	DOREPLIFETIME(ACombatEnemy, MaxHP);
	DOREPLIFETIME(ACombatEnemy, EnemyPlayMontageInfo);
}

void ACombatEnemy::BeginPlay()
{
	// reset HP to maximum
	CurrentHP = MaxHP;

	// we top the HP before BeginPlay so StateTree picks it up at the right value
	Super::BeginPlay();

	// get the life bar widget from the widget comp
	LifeBarWidget = Cast<UCombatLifeBar>(LifeBar->GetUserWidgetObject());
	check(LifeBarWidget);

	UpdateLifeBar();
}

void ACombatEnemy::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (!HasAuthority())
		return;

	// clear the death timer
	GetWorld()->GetTimerManager().ClearTimer(DeathTimer);
}

void ACombatEnemy::UpdateAttackWarpTarget(AActor* FocusedActor)
{
	if (!HasAuthority())
		return;

	if (!FocusedActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid focused target"));
		return;
	}

	Multicast_UpdateAttackWarpTarget(FocusedActor->GetActorLocation());
}

void ACombatEnemy::RemoveAttackWarpTarget()
{
	if (!HasAuthority())
		return;

	Multicast_RemoveAttackWarpTarget();
}

void ACombatEnemy::Server_PlayAnimMontage_Implementation(UAnimMontage* AnimMontage, float PlayRate, FName StartSectionName)
{
	EnemyPlayMontageInfo.Montage = AnimMontage;
	EnemyPlayMontageInfo.PlayRate = PlayRate;
	EnemyPlayMontageInfo.StartSectionName = StartSectionName;
	EnemyPlayMontageInfo.bRequestStop = false;

	if (const AGameStateBase* GameStateBase = UGameplayStatics::GetGameState(this))
	{
		EnemyPlayMontageInfo.TimeRequested = GameStateBase->GetServerWorldTimeSeconds();
	}
}

void ACombatEnemy::Multicast_PlayMontageSection_Implementation(FName MontageSectionName)
{
	if (!IsValid(EnemyPlayMontageInfo.Montage))
		return;

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_JumpToSection(MontageSectionName, EnemyPlayMontageInfo.Montage);
	}
}

void ACombatEnemy::Multicast_UpdateAttackWarpTarget_Implementation(const FVector_NetQuantize& WarpTargetLocation)
{
	MotionWarpingComponent->AddOrUpdateWarpTargetFromTransform("AttackWarpTarget", FTransform(GetActorRotation(), WarpTargetLocation, GetActorScale()));
}

void ACombatEnemy::Multicast_RemoveAttackWarpTarget_Implementation()
{
	MotionWarpingComponent->RemoveWarpTarget("AttackWarpTarget");
}

void ACombatEnemy::OnRep_CurrentHP()
{
	CurrentHP = FMath::Clamp(CurrentHP, 0.f, MaxHP);

	UpdateLifeBar();
}

void ACombatEnemy::OnRep_MaxHP()
{
	CurrentHP = FMath::Clamp(CurrentHP, 0.f, MaxHP);

	UpdateLifeBar();
}

void ACombatEnemy::OnRep_EnemyPlayMontageInfo()
{
	if (!IsValid(EnemyPlayMontageInfo.Montage))
		return;

	if (EnemyPlayMontageInfo.bRequestStop)
	{
		StopAnimMontage(EnemyPlayMontageInfo.Montage);
	}
	else
	{
		// We want to advance the montage to account for lag and sync up the animations everywhere as best we can
		// It's possible that because of extreme lag, or network relevancy meaning this is replicated long after
		// it was requested, that we don't need to play this montage.

		float PlayOffset = 0.f;
		const float Duration = EnemyPlayMontageInfo.Montage->GetPlayLength() / EnemyPlayMontageInfo.PlayRate;
		if (AGameStateBase* GameStateBase = UGameplayStatics::GetGameState(this))
		{
			PlayOffset = GameStateBase->GetServerWorldTimeSeconds() - EnemyPlayMontageInfo.TimeRequested;
			if (PlayOffset >= Duration)
			{
				// Skip, this play was requested too long ago
				return;
			}
		}

		// We need to use the lower level play montage function so we have access to start time
		if (USkeletalMeshComponent* CharacterMesh = GetMesh())
		{
			if (UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance())
			{
				const float TimeLeft = AnimInstance->Montage_Play(EnemyPlayMontageInfo.Montage, EnemyPlayMontageInfo.PlayRate, EMontagePlayReturnType::Duration, PlayOffset);

				// I think this possibly breaks the lag compensation, so maybe don't use this if you want good sync
				if (TimeLeft > 0.f && EnemyPlayMontageInfo.StartSectionName != NAME_None)
				{
					Multicast_PlayMontageSection(EnemyPlayMontageInfo.StartSectionName);

					AnimInstance->Montage_JumpToSection(EnemyPlayMontageInfo.StartSectionName, EnemyPlayMontageInfo.Montage);
				}
			}
		}
	}
}

void ACombatEnemy::UpdateLifeBar()
{
	if (LifeBarWidget)
	{
		LifeBarWidget->SetLifePercentage(CurrentHP / MaxHP);
	}
}
