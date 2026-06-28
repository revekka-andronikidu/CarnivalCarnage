// Copyright Abonneer, Inc. All Rights Reserved.


#include "PoleMovementComponent.h"
#include "BumperCarMovementComponent.h"
#include "BumperCar.h"

// Sets default values for this component's properties
UPoleMovementComponent::UPoleMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPoleMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPoleMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	UpdatePolePosition(DeltaTime);
}

void UPoleMovementComponent::SetupComponent(UBumperCarMovementComponent* movementComp, ABumperCar* ownerPtr)
{
	MovementComponentPtr = movementComp;
	OwnerPtr = ownerPtr;
}

void UPoleMovementComponent::ResetComponent()
{
	PolePosition = FVector::ZeroVector;
}

void UPoleMovementComponent::UpdatePolePosition(float deltaTime)
{
	// Update Y value
	if (MovementComponentPtr->GetIsBraking())
	{
		float currentSpeed = MovementComponentPtr->GetCurrentSpeed();
		
		if (currentSpeed < 500.f)
		{
			float speedPercentage = FMath::Clamp(currentSpeed / 500.f, 0.f, 1.f);
			PolePosition.Y = FMath::FInterpTo(PolePosition.Y, speedPercentage, deltaTime, PoleSpeed);
		}
		else
		{
			PolePosition.Y = FMath::FInterpTo(PolePosition.Y, 1.f, deltaTime, PoleSpeed);
		}
	}
	else
	{
		float currentSpeed = MovementComponentPtr->GetCurrentSpeed();
		
		float maxSpeed = MovementComponentPtr->GetMaxBoostSpeed();
		float speedPercentage = FMath::Clamp(FMath::Abs(currentSpeed) / maxSpeed, 0.f, 1.f);
		PolePosition.Y = FMath::FInterpTo(PolePosition.Y, -speedPercentage, deltaTime, PoleSpeed);
	}

	// Update X value
	if (MovementComponentPtr->GetIsSteering())
	{
		if (MovementComponentPtr->GetIsSteeringRight())
		{
			float currentSteeringSpeed = MovementComponentPtr->GetSteeringSpeed();
			float maxSteeringSpeed = MovementComponentPtr->GetMaxSteeringSpeed();
			float steeringSpeedPercentage = FMath::Clamp(FMath::Abs(currentSteeringSpeed) / maxSteeringSpeed, 0.f, 1.f);
			PolePosition.X = FMath::FInterpTo(PolePosition.X, -steeringSpeedPercentage / 2.f, deltaTime, TurningSpeed);
		}
		else
		{
			float currentSteeringSpeed = MovementComponentPtr->GetSteeringSpeed();
			float maxSteeringSpeed = MovementComponentPtr->GetMaxSteeringSpeed();
			float steeringSpeedPercentage = FMath::Clamp(FMath::Abs(currentSteeringSpeed) / maxSteeringSpeed, 0.f, 1.f);
			PolePosition.X = FMath::FInterpTo(PolePosition.X, steeringSpeedPercentage / 2.f, deltaTime, TurningSpeed);
		}
	}
	else
	{
		PolePosition.X = FMath::FInterpTo(PolePosition.X, 0.f, deltaTime, TurningSpeed);
	}
	OwnerPtr->OnPoleMovedEvent(PolePosition);
}
