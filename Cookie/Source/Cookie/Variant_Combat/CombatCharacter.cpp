// Copyright Epic Games, Inc. All Rights Reserved.

#include "CombatCharacter.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "CombatLifeBar.h"
#include "CombatPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Cookie.h"
#include "Core/CkGamePlayerState.h"
#include "Engine/DamageEvents.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "UI/CkNameTagWidget.h"

ACombatCharacter::ACombatCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	// bind the attack montage ended delegate
	OnAttackMontageEnded.BindUObject(this, &ACombatCharacter::AttackMontageEnded);

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.0f);

	// Configure character movement
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;

	// create the camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraBoom->TargetArmLength = DefaultCameraDistance;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bEnableCameraRotationLag = true;

	// create the orbiting camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// create the life bar widget component
	LifeBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("LifeBar"));
	LifeBar->SetupAttachment(RootComponent);

	NameTagWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Nameplate"));
	NameTagWidgetComponent->SetupAttachment(GetMesh());
	NameTagWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	NameTagWidgetComponent->SetDrawSize(FVector2D(200.f, 40.f));
	NameTagWidgetComponent->SetPivot(FVector2D(0.5f, 0.f));
	NameTagWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 110.f));
	NameTagWidgetComponent->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	NameTagWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NameTagWidgetComponent->SetReceivesDecals(false);
	NameTagWidgetComponent->SetTwoSided(true);

	// set the player tag
	Tags.Add(FName("Player"));
}

void ACombatCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ACombatCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ACombatCharacter::ComboAttackPressed()
{
	// route the input
	DoComboAttackStart();
}

void ACombatCharacter::ChargedAttackPressed()
{
	// route the input
	DoChargedAttackStart();
}

void ACombatCharacter::ChargedAttackReleased()
{
	// route the input
	DoChargedAttackEnd();
}

void ACombatCharacter::ToggleCamera()
{
	// call the BP hook
	BP_ToggleCamera();
}

void ACombatCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ACombatCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ACombatCharacter::DoComboAttackStart()
{
	Server_DoComboAttackStart();
}

void ACombatCharacter::Server_DoComboAttackStart_Implementation()
{
	if (bIsAttacking)
	{
		// Cache the input time so we can check it later
		CachedAttackInputTime = GetWorld()->GetTimeSeconds();
		return;
	}

	Server_ComboAttack();
}

void ACombatCharacter::DoComboAttackEnd()
{
	// stub
}

void ACombatCharacter::DoChargedAttackStart()
{
	Server_DoChargedAttackStart();
}

void ACombatCharacter::Server_DoChargedAttackStart_Implementation()
{
	bIsChargingAttack = true;

	if (bIsAttacking)
	{
		// Cache the input time so we can check it later
		CachedAttackInputTime = GetWorld()->GetTimeSeconds();
		return;
	}

	Server_ChargedAttack();
}

void ACombatCharacter::Server_ChargedAttack_Implementation()
{
	bIsAttacking = true;

	bHasLoopedChargedAttack = false;

	NotifyEnemiesOfIncomingAttack();

	if (!ChargedAttackMontage)
	{
		UE_LOG(LogCookie, Warning, TEXT("Missing ChargedAttackMontage"));
		return;
	}

	Server_PlayAnimMontage(ChargedAttackMontage);
	
	const float MontageLength = PlayAnimMontage(PlayMontageInfo.Montage, PlayMontageInfo.PlayRate, PlayMontageInfo.StartSectionName);
	if (MontageLength > 0.f)
	{
		if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
		{
			AnimInst->Montage_SetEndDelegate(OnAttackMontageEnded, ChargedAttackMontage);
		}
	}
}

void ACombatCharacter::DoChargedAttackEnd()
{
	Server_DoChargedAttackEnd();
}

void ACombatCharacter::Server_DoChargedAttackEnd_Implementation()
{
	bIsChargingAttack = false;

	if (bHasLoopedChargedAttack)
	{
		Server_CheckChargedAttack();
	}
}

void ACombatCharacter::Server_CheckChargedAttack_Implementation()
{
	bHasLoopedChargedAttack = true;

	// Jump to either the loop or the attack section depending on whether we're still holding the charge button
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		MontageSectionName = bIsChargingAttack ? ChargeLoopSection : ChargeAttackSection;

		AnimInstance->Montage_JumpToSection(MontageSectionName, ChargedAttackMontage);
	}
}

void ACombatCharacter::Server_CheckComboAttack_Implementation()
{
	if (bIsAttacking && !bIsChargingAttack)
	{
		// is the last attack input not stale?
		if (GetWorld()->GetTimeSeconds() - CachedAttackInputTime <= ComboInputCacheTimeTolerance)
		{
			CachedAttackInputTime = 0.f;

			++ComboCount;

			// do we still have a combo section to play?
			if (ComboCount < ComboSectionNames.Num())
			{
				// notify enemies they are about to be attacked
				NotifyEnemiesOfIncomingAttack();

				// jump to the next combo section
				if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
				{
					MontageSectionName = ComboSectionNames[ComboCount];

					AnimInstance->Montage_JumpToSection(MontageSectionName, ComboAttackMontage);
				}
			}
		}
	}
}

void ACombatCharacter::ResetHP()
{
	CurrentHP = MaxHP;
}

void ACombatCharacter::Server_ComboAttack_Implementation()
{
	bIsAttacking = true;

	ComboCount = 0;

	NotifyEnemiesOfIncomingAttack();

	if (!ComboAttackMontage)
	{
		UE_LOG(LogCookie, Warning, TEXT("Missing ComboAttackMontage"));
		return;
	}

	Server_PlayAnimMontage(ComboAttackMontage);

	const float MontageLength = PlayAnimMontage(PlayMontageInfo.Montage, PlayMontageInfo.PlayRate, PlayMontageInfo.StartSectionName);
	if (MontageLength > 0.f)
	{
		if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
		{
			AnimInst->Montage_SetEndDelegate(OnAttackMontageEnded, ComboAttackMontage);
		}
	}
}

void ACombatCharacter::Server_DoAttackTrace_Implementation(FName DamageSourceBone)
{
	// sweep for objects in front of the character to be hit by the attack
	TArray<FHitResult> OutHits;

	// start at the provided socket location, sweep forward
	const FVector TraceStart = GetMesh()->GetSocketLocation(DamageSourceBone);
	const FVector TraceEnd = TraceStart + (GetActorForwardVector() * MeleeTraceDistance);

	// check for pawn and world dynamic collision object types
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);

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
			// check if we've hit a damageable actor
			ICombatDamageable* Damageable = Cast<ICombatDamageable>(CurrentHit.GetActor());

			if (Damageable)
			{
				// knock upwards and away from the impact normal
				const FVector Impulse = (CurrentHit.ImpactNormal * -MeleeKnockbackImpulse) + (FVector::UpVector * MeleeLaunchImpulse);

				// pass the damage event to the actor
				Damageable->ApplyDamage(MeleeDamage, this, CurrentHit.ImpactPoint, Impulse);

				Client_PlayDamageDealtEffect(MeleeDamage, CurrentHit.ImpactPoint);
			}
		}
	}
}

void ACombatCharacter::AttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsAttacking = false;

	// Check if we have a non-stale cached input
	if (GetWorld()->GetTimeSeconds() - CachedAttackInputTime <= AttackInputCacheTimeTolerance)
	{
		if (bIsChargingAttack)
		{
			Server_ChargedAttack();
		}
		else
		{
			Server_ComboAttack();
		}
	}
}

void ACombatCharacter::DoAttackTrace(FName DamageSourceBone)
{
	if (!HasAuthority())
		return; // We only need to get the anim notify from the player character on the server

	Server_DoAttackTrace(DamageSourceBone);
}

void ACombatCharacter::CheckCombo()
{
	if (!HasAuthority())
		return; // We only need to get the anim notify from the player character on the server

	Server_CheckComboAttack();
}

void ACombatCharacter::CheckChargedAttack()
{
	if (!HasAuthority())
		return; // We only need to get the anim notify from the player character on the server

	Server_CheckChargedAttack();
}

void ACombatCharacter::Server_ApplyDamage_Implementation(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse)
{
	// pass the damage event to the actor
	FDamageEvent DamageEvent;
	const float ActualDamage = TakeDamage(Damage, DamageEvent, nullptr, DamageCauser);

	// only process knockback and effects if we received nonzero damage
	if (ActualDamage > 0.0f)
	{
		// apply the knockback impulse
		GetCharacterMovement()->AddImpulse(DamageImpulse, true);

		// is the character ragdolling?
		if (GetMesh()->IsSimulatingPhysics())
		{
			// apply an impulse to the ragdoll
			GetMesh()->AddImpulseAtLocation(DamageImpulse * GetMesh()->GetMass(), DamageLocation);
		}

		Multicast_PlayDamageReceivedEffect(ActualDamage, DamageLocation, DamageImpulse);
	}
}

void ACombatCharacter::Multicast_PlayDamageReceivedEffect_Implementation(float Damage, const FVector_NetQuantize& DamageLocation, const FVector_NetQuantize& DamageImpulse)
{
	BP_ReceivedDamage(Damage, DamageLocation, DamageImpulse.GetSafeNormal());
}

void ACombatCharacter::Client_PlayDamageDealtEffect_Implementation(float Damage, const FVector_NetQuantize& ImpactPoint)
{
	BP_DealtDamage(Damage, ImpactPoint);
}

void ACombatCharacter::NotifyEnemiesOfIncomingAttack()
{
	// sweep for objects in front of the character to be hit by the attack
	TArray<FHitResult> OutHits;

	// start at the actor location, sweep forward
	const FVector TraceStart = GetActorLocation();
	const FVector TraceEnd = TraceStart + (GetActorForwardVector() * DangerTraceDistance);

	// check for pawn object types only
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	// use a sphere shape for the sweep
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(DangerTraceRadius);

	// ignore self
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->SweepMultiByObjectType(OutHits, TraceStart, TraceEnd, FQuat::Identity, ObjectParams, CollisionShape, QueryParams))
	{
		// iterate over each object hit
		for (const FHitResult& CurrentHit : OutHits)
		{
			// check if we've hit a damageable actor
			ICombatDamageable* Damageable = Cast<ICombatDamageable>(CurrentHit.GetActor());

			if (Damageable)
			{
				// notify the enemy
				Damageable->NotifyDanger(GetActorLocation(), this);
			}
		}
	}
}

void ACombatCharacter::ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse)
{
	Server_ApplyDamage(Damage, DamageCauser, DamageLocation, DamageImpulse);
}

void ACombatCharacter::HandleDeath()
{
	if (!HasAuthority())
		return;

	GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &ACombatCharacter::RespawnCharacter, RespawnTime, false);

	Multicast_HandleDeath();
}

void ACombatCharacter::ApplyHealing(float Healing, AActor* Healer)
{
	// stub
}

void ACombatCharacter::NotifyDanger(const FVector& DangerLocation, AActor* DangerSource)
{
	// stub
}

void ACombatCharacter::RespawnCharacter()
{
	// destroy the character and let it be respawned by the Player Controller
	Destroy();
}

float ACombatCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!HasAuthority())
		return 0.f;
	
	// only process damage if the character is still alive
	if (CurrentHP <= 0.f)
		return 0.f;

	// reduce the current HP
	CurrentHP -= Damage;

	CurrentHP = FMath::Clamp(CurrentHP, 0.f, MaxHP);

	UpdateLifeBar();

	if (CurrentHP == 0.f)
	{
		HandleDeath();
	}
	else
	{
		// enable partial ragdoll physics, but keep the pelvis vertical
		GetMesh()->SetPhysicsBlendWeight(0.5f);
		GetMesh()->SetBodySimulatePhysics(PelvisBoneName, false);
	}

	return Damage;
}

void ACombatCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	// is the character still alive?
	if (CurrentHP >= 0.0f)
	{
		// disable ragdoll physics
		GetMesh()->SetPhysicsBlendWeight(0.0f);
	}
}

void ACombatCharacter::Server_PlayAnimMontage_Implementation(UAnimMontage* AnimMontage,
	float PlayRate,
	FName StartSectionName)
{
	PlayMontageInfo.Montage = AnimMontage;
	PlayMontageInfo.PlayRate = PlayRate;
	PlayMontageInfo.StartSectionName = StartSectionName;
	PlayMontageInfo.bRequestStop = false;

	if (const AGameStateBase* GameStateBase = UGameplayStatics::GetGameState(this))
	{
		PlayMontageInfo.TimeRequested = GameStateBase->GetServerWorldTimeSeconds();
	}
}

void ACombatCharacter::Server_StopAnimMontage_Implementation(UAnimMontage* AnimMontage)
{
	if (AnimMontage != PlayMontageInfo.Montage)
		return;

	PlayMontageInfo.bRequestStop = true;

	StopAnimMontage(AnimMontage);
}

void ACombatCharacter::OnRep_PlayMontageInfo()
{
	if (!IsValid(PlayMontageInfo.Montage))
		return;

	if (PlayMontageInfo.bRequestStop)
	{
		StopAnimMontage(PlayMontageInfo.Montage);
	}
	else
	{
		// We want to advance the montage to account for lag and sync up the animations everywhere as best we can
		// It's possible that because of extreme lag, or network relevancy meaning this is replicated long after
		// it was requested, that we don't need to play this montage.

		float PlayOffset = 0.f;
		const float Duration = PlayMontageInfo.Montage->GetPlayLength() / PlayMontageInfo.PlayRate;
		if (AGameStateBase* GameStateBase = UGameplayStatics::GetGameState(this))
		{
			PlayOffset = GameStateBase->GetServerWorldTimeSeconds() - PlayMontageInfo.TimeRequested;
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
				const float TimeLeft = AnimInstance->Montage_Play(PlayMontageInfo.Montage, PlayMontageInfo.PlayRate, EMontagePlayReturnType::Duration, PlayOffset);

				// I think this possibly breaks the lag compensation, so maybe don't use this if you want good sync
				if (TimeLeft > 0.f && PlayMontageInfo.StartSectionName != NAME_None)
				{
					AnimInstance->Montage_JumpToSection(PlayMontageInfo.StartSectionName, PlayMontageInfo.Montage);
				}
			}
		}
	}
}

void ACombatCharacter::OnRep_MontageSectionName()
{
	if (!IsValid(PlayMontageInfo.Montage))
		return;

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_JumpToSection(MontageSectionName, PlayMontageInfo.Montage);
	}
}

void ACombatCharacter::OnRep_CurrentHP()
{
	CurrentHP = FMath::Clamp(CurrentHP, 0.f, MaxHP);

	UpdateLifeBar();
}

void ACombatCharacter::OnRep_MaxHP()
{
	CurrentHP = FMath::Clamp(CurrentHP, 0.f, MaxHP);

	UpdateLifeBar();
}

void ACombatCharacter::Multicast_HandleDeath_Implementation()
{
	GetCharacterMovement()->DisableMovement();

	GetMesh()->SetSimulatePhysics(true);

	LifeBar->SetHiddenInGame(true);

	if (IsLocallyControlled())
	{
		GetCameraBoom()->TargetArmLength = DeathCameraDistance;
	}
}

void ACombatCharacter::Server_ApplyPlayerColorToMesh_Implementation(const FColor& PlayerColor)
{
	BP_ApplyPlayerColorToMesh(PlayerColor);
}

void ACombatCharacter::UpdateLifeBar()
{
	if (LifeBarWidget)
	{
		LifeBarWidget->SetLifePercentage(CurrentHP / MaxHP);
	}
}

void ACombatCharacter::RefreshNameTag()
{
	if (!NameTagWidget)
	{
		if (UUserWidget* NameTagUserWidget = NameTagWidgetComponent->GetUserWidgetObject())
		{
			NameTagWidget = Cast<UCkNameTagWidget>(NameTagUserWidget);
		}
	}

	const ACkGamePlayerState* NameTagPlayerState = Cast<ACkGamePlayerState>(GetPlayerState());
	if (NameTagWidget && NameTagPlayerState)
	{
		const FString RoleText = NameTagPlayerState->bIsHost ? " (Host)" : " (Client)";
		NameTagWidget->SetPlayerName(FText::FromString(NameTagPlayerState->GetPlayerName() + RoleText));
	}

	const bool bIsLocalControlled = IsLocallyControlled();
	if (NameTagWidgetComponent)
	{
		NameTagWidgetComponent->SetVisibility(!bIsLocalControlled);
	}
}

void ACombatCharacter::Server_RefreshNameTag_Implementation()
{
	RefreshNameTag();
}

void ACombatCharacter::BeginPlay()
{
	Super::BeginPlay();

	// get the life bar from the widget component
	LifeBarWidget = Cast<UCombatLifeBar>(LifeBar->GetUserWidgetObject());
	check(LifeBarWidget);

	// initialize the camera
	GetCameraBoom()->TargetArmLength = DefaultCameraDistance;

	// save the relative transform for the mesh so we can reset the ragdoll later
	MeshStartingTransform = GetMesh()->GetRelativeTransform();

	// set the life bar color
	LifeBarWidget->SetBarColor(LifeBarColor);

	// reset HP to maximum
	ResetHP();

	if (const APlayerState* PlayerStateBase = GetPlayerState())
	{
		const ACkGamePlayerState* GamePlayerState = Cast<ACkGamePlayerState>(PlayerStateBase);
		BP_ApplyPlayerColorToMesh(GamePlayerState->PlayerColor);
	}

	RefreshNameTag();
}

void ACombatCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// clear the respawn timer
	GetWorld()->GetTimerManager().ClearTimer(RespawnTimer);
}

void ACombatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACombatCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACombatCharacter::Look);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ACombatCharacter::Look);

		// Combo Attack
		EnhancedInputComponent->BindAction(ComboAttackAction, ETriggerEvent::Started, this, &ACombatCharacter::ComboAttackPressed);

		// Charged Attack
		EnhancedInputComponent->BindAction(ChargedAttackAction, ETriggerEvent::Started, this, &ACombatCharacter::ChargedAttackPressed);
		EnhancedInputComponent->BindAction(ChargedAttackAction, ETriggerEvent::Completed, this, &ACombatCharacter::ChargedAttackReleased);

		// Camera Side Toggle
		EnhancedInputComponent->BindAction(ToggleCameraAction, ETriggerEvent::Triggered, this, &ACombatCharacter::ToggleCamera);

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}
}

void ACombatCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (!HasAuthority())
		return;

	if (ACombatPlayerController* CombatPlayerController = Cast<ACombatPlayerController>(GetController()))
	{
		CombatPlayerController->SetRespawnTransform(GetActorTransform());
	}
}

void ACombatCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority())
		return;

	if (const APlayerState* PlayerStateBase = GetPlayerState())
	{
		const ACkGamePlayerState* GamePlayerState = Cast<ACkGamePlayerState>(PlayerStateBase);
		RefreshNameTag();
		BP_ApplyPlayerColorToMesh(GamePlayerState->PlayerColor);
	}
}

void ACombatCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Client-side initialization

	const APlayerState* PlayerStateBase = GetPlayerState();
	const ACkGamePlayerState* GamePlayerState = Cast<ACkGamePlayerState>(PlayerStateBase);

	RefreshNameTag();
	BP_ApplyPlayerColorToMesh(GamePlayerState->PlayerColor);

	if (IsLocallyControlled())
	{
		Server_RefreshNameTag();
		Server_ApplyPlayerColorToMesh(GamePlayerState->PlayerColor);
	}
}

void ACombatCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACombatCharacter, bIsAttacking);
	DOREPLIFETIME(ACombatCharacter, ComboCount);
	DOREPLIFETIME(ACombatCharacter, bIsChargingAttack);
	DOREPLIFETIME(ACombatCharacter, bHasLoopedChargedAttack);
	DOREPLIFETIME(ACombatCharacter, PlayMontageInfo);
	DOREPLIFETIME(ACombatCharacter, MontageSectionName);
	DOREPLIFETIME(ACombatCharacter, CurrentHP);
	DOREPLIFETIME(ACombatCharacter, MaxHP);
}
