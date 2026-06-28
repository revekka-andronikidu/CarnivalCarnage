// Copyright Abonneer, Inc. All Rights Reserved.


#include "BumperCarController.h"
#include "BumperCar.h"

// Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void ABumperCarController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		subsystem->AddMappingContext(BumperCarContextPtr, 0);
	}
}

void ABumperCarController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UEnhancedInputComponent* enhancedInputComponentPtr = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		if (ABumperCar* bumperCarPtr = Cast<ABumperCar>(GetPawn()))
		{
			//enhancedInputComponentPtr->BindAction(SteeringActionPtr, ETriggerEvent::Triggered, bumperCarPtr, &ABumperCar::SteeringAction);
			//enhancedInputComponentPtr->BindAction(AccelerateActionPtr, ETriggerEvent::Triggered, bumperCarPtr, &ABumperCar::AccelerateAction);
			//enhancedInputComponentPtr->BindAction(AccelerateActionPtr, ETriggerEvent::Completed, bumperCarPtr, &ABumperCar::CompletedAccelerating);
			//enhancedInputComponentPtr->BindAction(BrakeActionPtr, ETriggerEvent::Triggered, bumperCarPtr, &ABumperCar::BrakeAction);
			//enhancedInputComponentPtr->BindAction(BrakeActionPtr, ETriggerEvent::Completed, bumperCarPtr, &ABumperCar::CompletedBraking);
			//enhancedInputComponentPtr->BindAction(ExtinguishActionPtr, ETriggerEvent::Completed, bumperCarPtr, &ABumperCar::ServerExtinguishAction);
			//enhancedInputComponentPtr->BindAction(BoostActionPtr, ETriggerEvent::Triggered, bumperCarPtr, &ABumperCar::BoostAction);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(0, 10.f, FColor::Red, TEXT("CAST BUMPER CAR FAILED!!!"));
		}
	}
}
