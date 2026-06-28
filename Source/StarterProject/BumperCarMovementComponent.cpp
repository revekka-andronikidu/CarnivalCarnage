// Copyright Abonneer, Inc. All Rights Reserved.


#include "BumperCarMovementComponent.h"
#include "BumperCar.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "PlayerController/MainPlayerState.h"

DEFINE_LOG_CATEGORY(LogMovementComponent);
// Sets default values for this component's properties
UBumperCarMovementComponent::UBumperCarMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	// ...
}

void UBumperCarMovementComponent::ResetComponent()
{
	// Speed & Velocity Calculations
	CurrentSpeed = 0.f;
	LastPosition = FVector::ZeroVector;
	LastDeltaTime = 0.f;
	bIsReversing = false;
	bIsBoosting = false;

	// acceleration parameters
	bIsAccelerating = false;
	CurrentAcceleration = 0.f;
	
	// braking parameters
	bIsBraking = false;

	// steering parameters
	bSteeringDisabled = false;
	CurrentSteeringSpeed = 0.f;
	CurrentRotation = FRotator::ZeroRotator;
	DesiredRotation = FRotator::ZeroRotator;

	// drifting parameters
	bIsDrifting = false;
	CurrentRotationAngle = 0.f;

	// physics
	ImpulseVectorForce = FVector::ZeroVector;

	// settings
	bForceDisableAcceleration = false;
	AccumulatedDisabledAccelerationTime = 0.f;
	bForceDisableMovement = true;
	bMovementEnabled = true;
	AccumulatedDisabledTime = 0.f;
	bPerpetualDriving = false;

	// falling parameters
	bIsFalling = false;
	IsFallingSoundPlayed = false;
	bFallingDirectionSetup;
	FallingSpeed = 0.f;
	CornerVector.Empty();
	AccumulatedRotationTime = 0.f;
	StartQuat = FQuat{};
	FinalQuat = FQuat{};
	FirstUnsupportedCornerIndex = -1;
}


// Called when the game starts
void UBumperCarMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (auto car = Cast<ABumperCar>(GetOwner())) { OwnerPtr = car; }

}

void UBumperCarMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(UBumperCarMovementComponent, ReplicatedImpulseForce);
	DOREPLIFETIME(UBumperCarMovementComponent, ReplicatedCurrentSpeed);
}

// Called every frame
void UBumperCarMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bForceDisableMovement) { return; }

	if (!bMovementEnabled) 
	{ 
		DisableMovementTimer(DeltaTime);
		return; 
	}

	if (bForceDisableAcceleration) { DisableAccelerationTimer(DeltaTime); }

	CalculateSpeed(DeltaTime);
	Move(DeltaTime);
	RotateOnFloor();
	CheckForUnsupportedCorners();

	LastPosition = GetOwner()->GetActorLocation();
	LastDeltaTime = DeltaTime;

	if (FallingSpeed > 1500 && !IsFallingSoundPlayed)
	{
	 OwnerPtr->OnFallingEvent(true);
	 IsFallingSoundPlayed = true;
	 UE_LOG(LogMovementComponent, Log, TEXT("AAAAAAAAAA"));
	}
	else if(FallingSpeed == 0)
	{
		OwnerPtr->OnFallingEvent(false);
		IsFallingSoundPlayed = false;
	}

	GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Yellow, FString::SanitizeFloat(FallingSpeed));
}

void UBumperCarMovementComponent::Accelerate(float inputValue, bool isCompleted)
{
	if (!bMovementEnabled || bForceDisableAcceleration || bForceDisableMovement) { return; }
	
	bIsAccelerating = !isCompleted;
	if (bIsBoosting)
	{
		CurrentAcceleration = BoostMaxAcceleration;
	}
	else
	{
		float accelerationPercentage = FMath::Clamp(CurrentAcceleration + (inputValue * AccelerationStep) / MaxAcceleration, 0.f, 1.f);
		CurrentAcceleration = FMath::Lerp(MinAcceleration, MaxAcceleration, accelerationPercentage);
	}
}

void UBumperCarMovementComponent::Brake(float inputValue, bool isCompleted)
{
	if (!bMovementEnabled) { return; }
	if (bIsBraking == isCompleted) { bIsBraking = !isCompleted; }
	if (isCompleted) { return; }

	if (CurrentSpeed <= 0 and !bIsReversing) 
	{ 
		bIsReversing = true;
		
		ABumperCar* car = Cast<ABumperCar>(GetOwner());
		car->OnReversingEvent(true);
		car->LookBackwards();
		
		UE_LOG(LogMovementComponent, Log, TEXT("Reversing"));
	}
	else if (!bIsReversing)
	{
		CurrentAcceleration = FMath::Lerp(MinBrakeForce, MaxBrakeForce, inputValue);
	}
}

void UBumperCarMovementComponent::Steer(float inputValue)
{
	if (bSteeringDisabled || bIsFalling) { return; }
	if (inputValue >= -0.1f and inputValue <= 0.1f) 
	{ 
		bIsSteering = false; 
		return;
	}
	if (CurrentSpeed == 0.f) { return; }

	float percentage = FMath::Clamp(FMath::Abs(CurrentSpeed) / SpeedToBaseSteering, 0.f, 1.f);
	CurrentSteeringSpeed = FMath::Lerp(MaxSteeringSpeed, MinSteeringSpeed, percentage);

	FRotator rotation = GetOwner()->GetActorRotation();
	if (bIsReversing) { rotation.Yaw -= inputValue * CurrentSteeringSpeed * GetWorld()->GetDeltaSeconds(); }
	else { rotation.Yaw += inputValue * CurrentSteeringSpeed * GetWorld()->GetDeltaSeconds(); }
	
	CurrentRotation = rotation;
	GetOwner()->SetActorRotation(rotation);
	
	bIsSteering = true;
	if (inputValue > 0) { bIsSteeringRight = true; }
	else { bIsSteeringRight = false; }
}

void UBumperCarMovementComponent::Boost(bool isCompleted)
{
	if (bForceDisableAcceleration and bIsBoosting)
	{
		bIsBoosting = false;
		return;
	}
	if (!bMovementEnabled || bForceDisableAcceleration) { return; }
	if (bIsBoosting == isCompleted) { bIsBoosting = !isCompleted; }

	if (CurrentAcceleration < BoostMaxAcceleration)
	{
		CurrentAcceleration = BoostMaxAcceleration;
	}
}

void UBumperCarMovementComponent::DisableMovementBySeconds(float seconds)
{
	bMovementEnabled = false;
	DisabledTime = seconds;
	AccumulatedDisabledTime = 0.f;
	CurrentAcceleration = 0.f;
	CurrentSpeed = 0.f;
}

void UBumperCarMovementComponent::DisableMovement(bool disable)
{
	bForceDisableMovement = disable;
	UE_LOG(LogMovementComponent, Log, TEXT("Disable Movement: %s"), disable ? TEXT("true") : TEXT("false"));
}

bool UBumperCarMovementComponent::GetMovementEnabled() const
{
	if (bForceDisableMovement) { return bForceDisableMovement; }
	else { return bMovementEnabled; }
}

void UBumperCarMovementComponent::DisableAcceleration(float seconds)
{
	bForceDisableAcceleration = true;
	AccumulatedDisabledAccelerationTime = 0.f;
	DisabledAccelerationTime = seconds;
	bIsAccelerating = false;
}

void UBumperCarMovementComponent::IsSlimed(float seconds)
{
	float slimedSpeed = CurrentSpeed - CurrentSpeed / 4;
	CurrentSpeed = slimedSpeed;
	bForceDisableAcceleration = true;
	AccumulatedDisabledAccelerationTime = 0.f;
	DisabledAccelerationTime = seconds;
	bIsAccelerating = false;
}

void UBumperCarMovementComponent::DisableSteering(bool steeringEnabled)
{
	bSteeringDisabled = steeringEnabled;
}

void UBumperCarMovementComponent::Impulse(FVector force)
{
	ImpulseVectorForce = force;
	CurrentAcceleration = 0.f;
	CurrentSpeed = 0.f;
	DisableAcceleration(0.2f);
}

FVector UBumperCarMovementComponent::GetVelocity()
{
	FVector velocity = (GetOwner()->GetActorLocation() - LastPosition) / LastDeltaTime;
	return velocity;
}

float UBumperCarMovementComponent::GetCurrentSpeed() const
{
	//UE_LOG(LogMovementComponent, Log, TEXT("%s Current Speed Requested: %f"), *GetOwner()->GetName(), CurrentSpeed);
	return CurrentSpeed;
}

FVector UBumperCarMovementComponent::CalculateImpulseForces()
{
	if ((!bIsAccelerating and !bIsBraking) or bIsFalling)
	{
		ImpulseVectorForce *= DragFactor;
		return ImpulseVectorForce;
	}
	else
	{
		FVector NormalizedImpulseVector = ImpulseVectorForce.GetSafeNormal();
	
		float dot = FVector::DotProduct(NormalizedImpulseVector, GetOwner()->GetActorForwardVector());
		dot = FMath::Clamp(dot, -1.f, 1.f);
		float directionPercentage = (1.f - dot) / 2.f; // 0 = same direction, 1 = opposite direction
		
		float slowingDownFactor{};
		if (bIsAccelerating) 
		{ 
			float accelerationPercentage = CurrentAcceleration / MaxAcceleration;
			directionPercentage = FMath::Clamp(directionPercentage * accelerationPercentage, 0.f, 1.f);
			slowingDownFactor = FMath::Lerp(DragFactor, CounterDampingFactor, directionPercentage); 
		}
		if (bIsBraking) 
		{ 
			float brakingPercentage = CurrentAcceleration / MaxBrakeForce;
			directionPercentage = FMath::Clamp(directionPercentage * brakingPercentage, 0.f, 1.f);
			slowingDownFactor = FMath::Lerp(CounterDampingFactor, DragFactor, directionPercentage); 
		}
	
		ImpulseVectorForce *= slowingDownFactor;
		return ImpulseVectorForce;
	}
}

void UBumperCarMovementComponent::Move(float deltaTime)
{
	FVector currentLocation = GetOwner()->GetActorLocation();

	if (IsGrounded()) 
	{ 
		if (bIsFalling) 
		{ 
			bIsFalling = false; 
			FallingSpeed = 0.f;
			AccumulatedRotationTime = 0.f;
		}
		
		FVector impulseVector = FVector::ZeroVector;
		if (ImpulseVectorForce.Size() > 0.f)
		{
			impulseVector = CalculateImpulseForces();
		}

		// snapping to ground
		FVector nextLocation = currentLocation + (GetOwner()->GetActorForwardVector() * CurrentSpeed * deltaTime);
		nextLocation += impulseVector * deltaTime;
		FVector groundLocation = GetLocationSnappedToGround(nextLocation);
		GetOwner()->SetActorLocation(groundLocation);
	}
	else
	{
		if (!bIsFalling)
		{
			FallingSpeed = 0.f;
			bIsFalling = true;
			UE_LOG(LogMovementComponent, Log, TEXT("Started Falling"));
		}

		// impulse calculation
		FVector impulseVector = FVector::ZeroVector;
		if (ImpulseVectorForce.Size() > 0.f)
		{
			impulseVector = CalculateImpulseForces();
		}

		// movement direction calculation
		FVector velocityVector = FVector::ZeroVector;
		if (bIsReversing)
		{
			velocityVector = -GetOwner()->GetActorForwardVector() * FMath::Abs(CurrentSpeed);
		}
		else
		{
			velocityVector = GetOwner()->GetActorForwardVector() * FMath::Abs(CurrentSpeed);
		}
		FVector directionVector = FVector::ZeroVector;
		if (velocityVector.Size() > 0.f)
		{
			directionVector = velocityVector / velocityVector.Size();
		}
		FVector forward = FVector::ZeroVector; 
		if (velocityVector.Size() > 0.f)
		{
			forward = directionVector * (FMath::Abs(CurrentSpeed) * deltaTime);
		}

		// gravity calculation
		FallingSpeed += GravityForce * GravityMultiplier * deltaTime;
		FVector gravity = (FallingSpeed * deltaTime) * FVector{ 0.f, 0.f, -1.f };

		// all calculations put together
		FVector nextLocation = currentLocation + gravity + forward + (impulseVector * deltaTime);
		GetOwner()->SetActorLocation(nextLocation);
		
		RotateOnFalling(deltaTime);
	}
	
	//GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Yellow, FString::SanitizeFloat(CurrentSpeed));
}

void UBumperCarMovementComponent::CalculateSpeed(float deltaTime)
{
	if (bIsBoosting)
	{
		CalculateSpeedAcceleration(deltaTime, MaxReverseSpeed, MaxBoostSpeed);
	}
	else if (bIsReversing)
	{
		if (bIsBraking)
		{
			CurrentSpeed = FMath::Clamp(CurrentSpeed - (DecelerationSpeed * deltaTime), MaxReverseSpeed, MinSpeed);
		}
		else if (bIsAccelerating)
		{
			CurrentSpeed = FMath::Clamp(CurrentSpeed + (CurrentAcceleration * deltaTime), MaxReverseSpeed, MinSpeed);
		}
		else
		{
			CurrentSpeed = FMath::Clamp(CurrentSpeed + (DecelerationSpeed * deltaTime), MaxReverseSpeed, MinSpeed);
		}
		if (CurrentSpeed >= 0 and bIsReversing)
		{
			bIsReversing = false;

			ABumperCar* car = Cast<ABumperCar>(GetOwner());
			car->OnReversingEvent(false);
			car->ResetBackWardsLookingCamera();

			UE_LOG(LogMovementComponent, Log, TEXT("Stopped Reversing"));
		}
	}
	else if (bIsAccelerating || bIsBraking)
	{
		if (CurrentSpeed > MaxSpeed)
		{
			CalculateSpeedDeceleration(deltaTime, MinSpeed, MaxBoostSpeed);
		}
		else
		{
			CalculateSpeedAcceleration(deltaTime, MaxReverseSpeed, MaxSpeed);
		}
	}
	else
	{
		CalculateSpeedDeceleration(deltaTime, MinSpeed, MaxBoostSpeed);
	}

	ABumperCar* ownerPawn = Cast<ABumperCar>(GetOwner());
	ReplicatedCurrentSpeed = CurrentSpeed;
	if ((!GetOwner()->HasAuthority() and ownerPawn->IsLocallyControlled())) { Server_SetSpeed(CurrentSpeed); }
}

bool UBumperCarMovementComponent::IsGrounded() const
{	
	FVector playerFrontLocation = GetOwner()->GetActorLocation() + (GetOwner()->GetActorForwardVector() * 20.f);
	FVector playerBackLocation = GetOwner()->GetActorLocation() + (-GetOwner()->GetActorForwardVector() * 20.f);
	FVector frontBeginTrace = playerFrontLocation + GroundCheckDistance * GetOwner()->GetActorUpVector();
	FVector backBeginTrace = playerBackLocation + GroundCheckDistance * GetOwner()->GetActorUpVector();
	FVector frontEndTrace = playerFrontLocation - GroundCheckDistance * GetOwner()->GetActorUpVector();
	FVector backEndTrace = playerBackLocation - GroundCheckDistance * GetOwner()->GetActorUpVector();
	
	FCollisionQueryParams params{};
	params.AddIgnoredActor(GetOwner());

	FHitResult hitFront{};
	FHitResult hitBack{};
	GetWorld()->LineTraceSingleByChannel(hitFront, frontBeginTrace, frontEndTrace, ECollisionChannel::ECC_GameTraceChannel1, params);
	GetWorld()->LineTraceSingleByChannel(hitBack, backBeginTrace, backEndTrace, ECollisionChannel::ECC_GameTraceChannel1, params);
	
	if (hitFront.bBlockingHit or hitBack.bBlockingHit)
	{ 
		return true; 
	}
	else 
	{
		return false; 
	}
}

FVector UBumperCarMovementComponent::GetLocationSnappedToGround(FVector nextPosition) const
{
	FVector beginTrace = nextPosition + GroundCheckDistance * GetOwner()->GetActorUpVector();
	FVector endTrace = nextPosition - GroundCheckDistance * GetOwner()->GetActorUpVector();
	FHitResult hit{};
		
	FCollisionQueryParams parameters{};
	parameters.AddIgnoredActor(GetOwner());

	GetWorld()->LineTraceSingleByChannel(hit, beginTrace, endTrace, ECollisionChannel::ECC_GameTraceChannel1, parameters);
	if (hit.bBlockingHit)
	{
		return hit.ImpactPoint + (hit.ImpactNormal * SnappingOffset);
	}
	else
	{
		return nextPosition;
	}
}

int UBumperCarMovementComponent::FindFirstUnsupportedCorner()
{
	auto collisionBox = OwnerPtr->GetCollisionBox();
	auto transform = OwnerPtr->GetTransform();
	FVector extent = collisionBox->GetScaledBoxExtent();
	extent.Z = 0.f;

	FVector bufferLocation;
	CornerVector.Empty();
	bufferLocation = FVector{ extent.X, -extent.Y, 0 };
	CornerVector.Add(transform.TransformPosition(bufferLocation));
	bufferLocation = FVector{ extent.X, extent.Y, 0 };
	CornerVector.Add(transform.TransformPosition(bufferLocation));
	bufferLocation = FVector{ -extent.X, extent.Y, 0 };
	CornerVector.Add(transform.TransformPosition(bufferLocation));
	bufferLocation = FVector{ -extent.X, -extent.Y, 0 };
	CornerVector.Add(transform.TransformPosition(bufferLocation));
	
	for (int index{0}; index < CornerVector.Num(); ++index)
	{
		FVector endTrace = CornerVector[index] - FVector{0.f, 0.f, GroundCheckDistance};

		FCollisionQueryParams params{};
		params.AddIgnoredActor(GetOwner());

		FHitResult hit{};
		GetWorld()->LineTraceSingleByChannel(hit, CornerVector[index], endTrace, ECollisionChannel::ECC_GameTraceChannel1, params);

		if (!hit.bBlockingHit) 
		{ 
			return index; 
		}
	}
	return -1;
}

void UBumperCarMovementComponent::CheckForUnsupportedCorners()
{
	if (bIsFalling) { return; }

	if ((FirstUnsupportedCornerIndex == -1 and FindFirstUnsupportedCorner() != -1) or
		(FirstUnsupportedCornerIndex != -1 and FindFirstUnsupportedCorner() == -1 ))
	{
		FirstUnsupportedCornerIndex = FindFirstUnsupportedCorner();
	}
}

void UBumperCarMovementComponent::RotateOnFloor()
{
	FVector playerLocation = GetOwner()->GetActorLocation();
	FVector endTrace = playerLocation - FVector{ 0, 0, GroundCheckDistance };

	FCollisionQueryParams params{};
	params.AddIgnoredActor(GetOwner());

	FHitResult hit{};
	GetWorld()->LineTraceSingleByChannel(hit, playerLocation, endTrace, ECollisionChannel::ECC_GameTraceChannel1, params);

	if (hit.bBlockingHit)
	{	
		DesiredRotation = FRotationMatrix::MakeFromZX(hit.ImpactNormal, GetOwner()->GetActorForwardVector()).Rotator();
		CurrentRotation = GetOwner()->GetActorRotation();

		FRotator interpolatedRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, GetWorld()->GetDeltaSeconds(), RotationInterpolationSpeed);
		GetOwner()->SetActorRotation(interpolatedRotation);
		
		if (bIsFalling)
		{
			UE_LOG(LogMovementComponent, Log, TEXT("Rotating while falling"));
		}
	}
}

void UBumperCarMovementComponent::RotateOnFalling(float deltaTime)
{
	if (GetOwner()->GetActorLocation().Z > -5000) { return; }
	
	if (CornerVector.Num() <= 0 || FirstUnsupportedCornerIndex == -1) { return; }

	if (!bFallingDirectionSetup)
	{
		FVector lookAtDirection = (CornerVector[FirstUnsupportedCornerIndex] - GetOwner()->GetActorLocation()).GetSafeNormal();
		FVector desiredLookAtDirection = FVector::DownVector.GetSafeNormal();
		
		FQuat rotationQuat = FQuat::FindBetweenNormals(lookAtDirection, desiredLookAtDirection);

		StartQuat = GetOwner()->GetActorQuat();
		FinalQuat = rotationQuat * StartQuat;
		bFallingDirectionSetup = true;
	}
	
	AccumulatedRotationTime = FMath::Clamp(AccumulatedRotationTime + deltaTime, 0.f, SecondsToRotateFully);
	FRotator deltaQuat = FQuat::Slerp(StartQuat, FinalQuat, AccumulatedRotationTime / SecondsToRotateFully).Rotator();

	GetOwner()->SetActorRotation(deltaQuat);
}

void UBumperCarMovementComponent::DisableMovementTimer(float deltaTime)
{
	AccumulatedDisabledTime += deltaTime;
	if (AccumulatedDisabledTime >= DisabledTime)
	{
		bMovementEnabled = true;
	}
}

void UBumperCarMovementComponent::DisableAccelerationTimer(float deltaTime)
{
	AccumulatedDisabledAccelerationTime += deltaTime;
	if (AccumulatedDisabledAccelerationTime >= DisabledAccelerationTime)
	{
		bForceDisableAcceleration = false;
	}
}

void UBumperCarMovementComponent::CalculateSpeedAcceleration(float deltaTime, float minimumSpeed, float maximumSpeed)
{
	if (bPerpetualDriving)
	{
		CurrentSpeed = FMath::Clamp(CurrentSpeed + (CurrentAcceleration * deltaTime), PerpetualMinSpeed, maximumSpeed);
	}
	else
	{
		CurrentSpeed = FMath::Clamp(CurrentSpeed + (CurrentAcceleration * deltaTime), minimumSpeed, maximumSpeed);
	}
}

void UBumperCarMovementComponent::CalculateSpeedDeceleration(float deltaTime, float minimumSpeed, float maximumSpeed)
{
	if (bPerpetualDriving)
	{
		CurrentSpeed = FMath::Clamp(CurrentSpeed - (DecelerationSpeed * deltaTime), PerpetualMinSpeed, maximumSpeed);
	}
	else
	{
		CurrentSpeed = FMath::Clamp(CurrentSpeed - (DecelerationSpeed * deltaTime), minimumSpeed, maximumSpeed);
	}
}

void UBumperCarMovementComponent::OnRep_SetCurrentSpeed()
{
	if (Cast<ABumperCar>(GetOwner())->IsLocallyControlled()) { return; }
	CurrentSpeed = ReplicatedCurrentSpeed;
}

void UBumperCarMovementComponent::Server_SetSpeed_Implementation(float speed)
{
	CurrentSpeed = speed;
}

void UBumperCarMovementComponent::Multicast_SetSpeed_Implementation(float speed)
{
	CurrentSpeed = speed;
}
