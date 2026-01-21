// Copyright Epic Games, Inc. All Rights Reserved.

#include "CookieCharacter.h"

#include "Actors/CkCookiePickup.h"
#include "Camera/CameraComponent.h"
#include "Core/CkGamePlayerState.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Cookie.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundBase.h"
#include "UI/CkNameTagWidget.h"

ACookieCharacter::ACookieCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	OverlapBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	OverlapBoxComponent->SetupAttachment(RootComponent);
	OverlapBoxComponent->SetBoxExtent(FVector(48.f, 48.f, 86.f));
	OverlapBoxComponent->SetRelativeLocation(FVector(50.f, 0.f, 0.f));
	OverlapBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapBoxComponent->SetCollisionProfileName("OverlapAllDynamic");
	OverlapBoxComponent->SetGenerateOverlapEvents(true);
	OverlapBoxComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	OverlapBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACookieCharacter::OnOverlapBegin);

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

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

	Cookies = 0;

	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);
}

void ACookieCharacter::BeginPlay()
{
	Super::BeginPlay();

	RefreshNameTag();
}

void ACookieCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACookieCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ACookieCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACookieCharacter::Look);
	}
	else
	{
		UE_LOG(LogCookie, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ACookieCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ACookieCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ACookieCharacter::DoMove(float Right, float Forward)
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

void ACookieCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ACookieCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void ACookieCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void ACookieCharacter::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACookieCharacter, CurrentHealth);
	DOREPLIFETIME(ACookieCharacter, Cookies);
}

void ACookieCharacter::OnHealthUpdate()
{
	if (IsLocallyControlled())
	{
		FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

		if (CurrentHealth <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
		}
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}
}

void ACookieCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void ACookieCharacter::SetCurrentHealth(float healthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
		OnHealthUpdate();
	}
}

float ACookieCharacter::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float damageApplied = CurrentHealth - DamageTaken;
	SetCurrentHealth(damageApplied);
	return damageApplied;
}

void ACookieCharacter::OnCookiesUpdate()
{
	if (IsLocallyControlled())
	{
		FString cookiesMessage = FString::Printf(TEXT("You now have %i cookies."), Cookies);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, cookiesMessage);
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		FString cookiesMessage = FString::Printf(TEXT("%s now has %i cookies."), *GetFName().ToString(), Cookies);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, cookiesMessage);
	}
}

void ACookieCharacter::OnRep_Cookies()
{
	OnCookiesUpdate();
}

void ACookieCharacter::SetCookies(int CookiesValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		Cookies = FMath::Clamp(CookiesValue, 0, CookiesValue);
		OnCookiesUpdate();
	}
}

void ACookieCharacter::OnOverlapBegin(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/, bool /*bFromSweep*/, const FHitResult& /*SweepResult*/)
{
	if (!OtherActor || OtherActor == this)
		return;

	if (HasAuthority())
	{
		if (Cast<ACkCookiePickup>(OtherActor) != nullptr)
		{
			const FVector actorLocation = OtherActor->GetActorLocation();
			MulticastPickupCookie(actorLocation);
			OtherActor->Destroy();

			SetCookies(Cookies + 1);
		}
	}
}

void ACookieCharacter::MulticastPickupCookie_Implementation(const FVector_NetQuantize& Location)
{
	UGameplayStatics::PlaySoundAtLocation(
		this,
		CookiePickupSound,
		Location);
}

void ACookieCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	RefreshNameTag();

	Server_RefreshNameTag();
}

void ACookieCharacter::RefreshNameTag()
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
		const FString RoleText = NameTagPlayerState->bIsHost ? "-Host" : "-Client";
		NameTagWidget->SetPlayerName(FText::FromString(NameTagPlayerState->GetPlayerName() + RoleText));

		/*const FLinearColor TeamColor = GetTeamColorFromPS(NameTagPlayerState);
		NameTagWidget->SetNameColor(TeamColor);*/
	}

	const bool bIsLocalControlled = IsLocallyControlled();
	if (NameTagWidgetComponent)
	{
		NameTagWidgetComponent->SetVisibility(!bIsLocalControlled);
	}
}

void ACookieCharacter::Server_RefreshNameTag_Implementation()
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
		const FString RoleText = NameTagPlayerState->bIsHost ? "-Host" : "-Client";
		NameTagWidget->SetPlayerName(FText::FromString(NameTagPlayerState->GetPlayerName() + RoleText));

		/*const FLinearColor TeamColor = GetTeamColorFromPS(NameTagPlayerState);
		NameTagWidget->SetNameColor(TeamColor);*/
	}

	const bool bIsLocalControlled = IsLocallyControlled();
	if (NameTagWidgetComponent)
	{
		NameTagWidgetComponent->SetVisibility(!bIsLocalControlled);
	}
}
