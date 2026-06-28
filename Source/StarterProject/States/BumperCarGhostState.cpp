// Copyright Abonneer, Inc. All Rights Reserved.


#include "BumperCarGhostState.h"
#include "StarterProject/BumperCar.h"
#include "StarterProject/BumperCarComponent.h"
#include "StarterProject/BumperCarMovementComponent.h"
#include "StarterProject/Trail.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

DEFINE_LOG_CATEGORY(LogGhostState);

TObjectPtr<UBumperCarState> UBumperCarGhostState::HandleState(float deltaTime, AActor* ownerPtr)
{
	return nullptr;
}

void UBumperCarGhostState::OnEnter(AActor* ownerPtr)
{
	// here you enter what happens when you become a ghost

	StateType = EBumperCarStateType::Ghost;

	ABumperCar* car = Cast<ABumperCar>(ownerPtr);
	car->Multicast_OnGhostEvent(true);

	if (car->GetTrail())
	{
		car->GetTrail()->Multicast_SwitchTrailEffect(true);
	}

	car->Multicast_SetCollisionProfile(FName("Car_Ghost"));

	UE_LOG(LogGhostState, Log, TEXT("Player %s entered ghost state."), *ownerPtr->GetName());
	GEngine->AddOnScreenDebugMessage(3, 2.f, FColor::Green, "Entering Ghost State");
}


void UBumperCarGhostState::OnExit(AActor* ownerPtr)
{
	ABumperCar* car = Cast<ABumperCar>(ownerPtr);

	
	car->Multicast_SetCollisionProfile(FName("Car_Alive"));

	car->Multicast_OnGhostEvent(false);

	UE_LOG(LogGhostState, Log, TEXT("Player %s exited ghost state."), *ownerPtr->GetName());
}

