// Copyright Epic Games, Inc. All Rights Reserved.
//Testing
#include "SuperSprintShowCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ASuperSprintShowCharacter //////////


ASuperSprintShowCharacter::ASuperSprintShowCharacter()
{
	// Basic Character Replication
	bReplicates = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate
	
	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 250.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	SpeedIncreaseRate = 100.0f;
	MaxSpeed = 1000.0f; // Default max speed

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ASuperSprintShowCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Store the original height of the capsule component
	OriginalHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ASuperSprintShowCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsSliding);

}

void ASuperSprintShowCharacter::StartCrouch()
{
	Crouch();
}

void ASuperSprintShowCharacter::StopCrouch()
{
	UnCrouch();
}
//////////////////////////////////////////////////////////////////////////
// Input

void ASuperSprintShowCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ASuperSprintShowCharacter::StartCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ASuperSprintShowCharacter::StopCrouch);


		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASuperSprintShowCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASuperSprintShowCharacter::Look);

		// Sliding
		EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Started, this, &ASuperSprintShowCharacter::StartSlide);

		EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Completed, this, &ASuperSprintShowCharacter::EndSlide);

	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASuperSprintShowCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}

	// Check if there's significant movement input
	if (!Value.Get<FVector2D>().IsNearlyZero())
	{
		// Gradually increase speed
		//float NewSpeed = FMath::Min(GetCharacterMovement()->MaxWalkSpeed + GetWorld()->DeltaTimeSeconds * SpeedIncreaseRate, MaxSpeed);
		//GetCharacterMovement()->MaxWalkSpeed = NewSpeed;

		if (HasAuthority())
		{
			ServerIncreaseSpeed();
		}
	}
}

void ASuperSprintShowCharacter::OnRep_ReplicatedSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = ReplicatedSpeed;
}

void ASuperSprintShowCharacter::ServerIncreaseSpeed_Implementation()
{
	if (!bIsSliding)
	{
		// Gradually increase speed
		float NewSpeed = FMath::Min(GetCharacterMovement()->MaxWalkSpeed + GetWorld()->DeltaTimeSeconds * SpeedIncreaseRate, MaxSpeed);
		GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
		ReplicatedSpeed = NewSpeed;
	}
}

void ASuperSprintShowCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASuperSprintShowCharacter::StartSlide()
{
	if (!HasAuthority())
	{
		ServerStartSlide();
	}
}

void ASuperSprintShowCharacter::ServerStartSlide_Implementation()
{	
		// Check if the character is in a state to slide, like running
		if (CanSlide())
		{
			// Save the current running speed
			PreSlideSpeed = GetCharacterMovement()->MaxWalkSpeed;

			// Change movement properties, like speed and friction
			GetCharacterMovement()->MaxWalkSpeed = SlideSpeed;
			GetCharacterMovement()->GroundFriction = SlideFriction;

			// Flag to indicate the character is sliding
			bIsSliding = true;

			// Trigger slide animation
			// Need an animation set up in animation blueprint
			// PlaySlideAnimation();

			// Additional logic (like a timer to end slide) goes here

			// Reduce capsule height
			GetCapsuleComponent()->SetCapsuleHalfHeight(OriginalHeight * 0.30f, true);
		}
}

void ASuperSprintShowCharacter::ServerEndSlide_Implementation()
{
	// Reset movement properties
	GetCharacterMovement()->MaxWalkSpeed = PreSlideSpeed;
	GetCharacterMovement()->GroundFriction = NormalFriction;

	// Flag to indicate that character is no longer sliding
	bIsSliding = false;

	// End slide animation
	// StopSlideAnimation();

	// Additional logic goes here

	// Reset capsule height
	GetCapsuleComponent()->SetCapsuleHalfHeight(OriginalHeight, true);
}

void ASuperSprintShowCharacter::EndSlide()
{
	if (!HasAuthority())
	{
		ServerEndSlide();
	}
}

bool ASuperSprintShowCharacter::CanSlide()
{
	return GetCharacterMovement()->IsMovingOnGround();
}
