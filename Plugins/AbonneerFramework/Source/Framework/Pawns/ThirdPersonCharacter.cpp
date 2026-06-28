#include "ThirdPersonCharacter.h"

// Components
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "SmoothSync.h"

// Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

DEFINE_LOG_CATEGORY(LogThirdPersonCharacter);

//////////////////////////////////////////////////////////////////////////
// Default
AThirdPersonCharacter::AThirdPersonCharacter(const FObjectInitializer& objectInitializer)
{
	// Construct the camera boom
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetCapsuleComponent());
	SpringArm->SetRelativeLocation(FVector(0.0f, 75.0f, 0.0f));
	SpringArm->SetRelativeRotation(FRotator::MakeFromEuler(FVector(0.0f, 0.0f, 90.0f)));
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->SocketOffset.Z = 150.0f;
	SpringArm->bDoCollisionTest = true;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritRoll = false;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraLagSpeed = 10.0f;
	SpringArm->CameraRotationLagSpeed = 10.0f;
	SpringArm->CameraLagMaxDistance = 50.0f;

	// Create Camera
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(SpringArm);
	ThirdPersonCamera->bAutoActivate = true;
	
	// Update Third Person Mesh Properties
	if (GetMesh())
	{
		GetMesh()->SetOwnerNoSee(false);
		GetMesh()->bCastDynamicShadow = true;
		GetMesh()->CastShadow = true;
		GetMesh()->SetRelativeRotation(FRotator::MakeFromEuler(FVector(0.f, 0.f, -90.f)));
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -95.f));
		GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetMesh()->SetGenerateOverlapEvents(true);
	}
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(30.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));

	// Construct SmoothSync
	SmoothSync = CreateDefaultSubobject<USmoothSync>(TEXT("SmoothSync"));
	ACharacter::SetReplicateMovement(false); // A bit confusing, but we don't want Unreal to handle movement replication, since SmoothSync takes care of this.

	// Net relevancy
	bReplicates = true;
	bAlwaysRelevant = true;
	SetNetCullDistanceSquared(FLT_MAX); // Could optimize this later on
	NetDormancy = ENetDormancy::DORM_Never;

	// Movement Properties
	if (UCharacterMovementComponent* movementComponent = GetCharacterMovement())
	{
		// Default Movement Speed
		movementComponent->MaxWalkSpeed = 300.0f;
		movementComponent->MaxWalkSpeedCrouched = 200.0f;
		movementComponent->AirControl = 0.95f;

		// Modify the NavAgentProperties via the CharacterMovementComponent
		movementComponent->NavAgentProps.bCanWalk = true;
		movementComponent->NavAgentProps.bCanJump = true;
		movementComponent->NavAgentProps.bCanCrouch = true;

		// Allow the spring arm to control pitch/yaw instead of the character
		movementComponent->bOrientRotationToMovement = false;
	}

	// Character Default Properties
	bUseControllerRotationPitch = false; // Allow pitch rotation control
	bUseControllerRotationYaw = true; // Allow yaw rotation control
	bUseControllerRotationRoll = false; // Disable roll unless needed

	//// Default Property Values
	// Sprinting
	bIsSprintingEnabled = false;
	WalkingMovementSpeed = 300;
	SprintingMovementSpeed = 600;
	
	// Camera
	ControlledPitch = 0.0f;
	MaxPitchAngle = 70.0f;
	MinMaxSpringArmLength = FVector2D(100.0f, 300.0f);
}

void AThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Get Default Walking Speed
	auto movement = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (movement)
	{
		WalkingMovementSpeed = movement->MaxWalkSpeed;
	}
}

//////////////////////////////////////////////////////////////////////////
// Default Setup
void AThirdPersonCharacter::PossessedBy(AController* newController)
{
	Super::PossessedBy(newController);

}
void AThirdPersonCharacter::OnPlayerStateChanged(APlayerState* newPlayerState, APlayerState* oldPlayerState)
{
	Super::OnPlayerStateChanged(newPlayerState, oldPlayerState);

}

//////////////////////////////////////////////////////////////////////////
// Input
void AThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* inputComponent)
{
	Super::SetupPlayerInputComponent(inputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(inputComponent))
	{
		//Jumping
		enhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::StartJump);
		enhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Move);
		enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AThirdPersonCharacter::Move);

		//Looking
		enhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Look);

		//Sprinting
		enhancedInputComponent->BindAction(SprintingAction, ETriggerEvent::Started, this,
										   &AThirdPersonCharacter::StartSprinting);
		enhancedInputComponent->BindAction(SprintingAction, ETriggerEvent::Completed, this,
										   &AThirdPersonCharacter::StopSprinting);

		//Crouching
		enhancedInputComponent->BindAction(CrouchingAction, ETriggerEvent::Started, this,
										   &AThirdPersonCharacter::StartCrouch);
		enhancedInputComponent->BindAction(CrouchingAction, ETriggerEvent::Completed, this,
										   &AThirdPersonCharacter::StopCrouch);
	}
}

void AThirdPersonCharacter::Move(const FInputActionValue& value)
{
	// input is a Vector2D
	FVector2D movementVector = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), movementVector.Y);
		AddMovementInput(GetActorRightVector(), movementVector.X);
	}
}
void AThirdPersonCharacter::Look(const FInputActionValue& value)
{
	FVector2D lookAxisVector = value.Get<FVector2D>();

	if (Controller)
	{
		// Handle Yaw (Horizontal Rotation)
		AddControllerYawInput(lookAxisVector.X);

		// Manually Clamp the Pitch to Prevent Looping
		ControlledPitch = FMath::Clamp(ControlledPitch - lookAxisVector.Y, -MaxPitchAngle,
									   MaxPitchAngle);

		// Combine Controller Yaw and Custom Clamped Pitch
		FRotator newControlRotation = Controller->GetControlRotation();
		newControlRotation.Pitch = ControlledPitch; // Use our manual pitch

		Controller->SetControlRotation(newControlRotation);

		// Update Third Person Camera (Spring Arm's Target Length)
		float targetLength = FMath::Lerp(MinMaxSpringArmLength.Y, MinMaxSpringArmLength.X,
										 FMath::Abs(ControlledPitch) / MaxPitchAngle);
		SpringArm->TargetArmLength = targetLength;
	}
}
void AThirdPersonCharacter::StartJump()
{
	Jump();
}
void AThirdPersonCharacter::StartSprinting()
{
	if (!bIsSprintingEnabled) { return; }

	auto movement = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (movement)
	{
		movement->MaxWalkSpeed = SprintingMovementSpeed;
	}
}
void AThirdPersonCharacter::StopSprinting()
{
	if (!bIsSprintingEnabled) { return; }

	auto movement = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (movement)
	{
		movement->MaxWalkSpeed = WalkingMovementSpeed;
	}
}
void AThirdPersonCharacter::StartCrouch()
{
	Crouch();
}
void AThirdPersonCharacter::StopCrouch()
{
	UnCrouch();
}
void AThirdPersonCharacter::OnStartCrouch(float halfHeightAdjust, float scaledHalfHeightAdjust)
{
	Super::OnStartCrouch(halfHeightAdjust, scaledHalfHeightAdjust);

	// Should probably do this animation based, but for sake of demo
	if (GetMesh())
	{
		GetMesh()->AddRelativeLocation(FVector(0.0f, 0.0f, -halfHeightAdjust));
	}
}
void AThirdPersonCharacter::OnEndCrouch(float halfHeightAdjust, float scaledHalfHeightAdjust)
{
	Super::OnEndCrouch(halfHeightAdjust, scaledHalfHeightAdjust);

	// Should probably do this animation based, but for sake of demo
	if (GetMesh())
	{
		GetMesh()->AddRelativeLocation(FVector(0.0f, 0.0f, halfHeightAdjust));
	}
}