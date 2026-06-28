// Copyright Abonneer, Inc. All Rights Reserved.


#include "BumperCarNeutralState.h"
#include "StarterProject/TrailCollisionComponent.h"
#include "StarterProject/DeathBox.h"
#include "BumperCarOnFireState.h"
#include "BumperCarOnSlimedState.h"
#include "BumperCarGhostState.h"
#include "StarterProject/BumperCar.h"
#include "StarterProject/HealthComponent.h"


TObjectPtr<UBumperCarState> UBumperCarNeutralState::HandleState(float deltaTime, AActor* ownerPtr)
{
	if (!bTrailEventsSetup) { SetupTrailEvents(); }
	CalculateStartHealingTime(deltaTime);

	if (bOnFire) { return NewObject<UBumperCarOnFireState>(); }
	if (bIsSlimed) { return NewObject<UBumperCarOnSlimedState>(); }
	if (bIsDead) { return NewObject<UBumperCarGhostState>(); }
	return nullptr;
}

void UBumperCarNeutralState::OnEnter(AActor* ownerPtr)
{
	StateType = EBumperCarStateType::Neutral;
	OwnerPtr = ownerPtr;
	GEngine->AddOnScreenDebugMessage(3, 2.f, FColor::Green, "Entering Neutral State");

	ABumperCar* car = Cast<ABumperCar>(ownerPtr);
	car->Multicast_SetCollisionProfile(FName("Car_Alive"));

	TimerStarted = true;
	UE_LOG(LogTemp, Log, TEXT("Player %s entered neutral state."), *ownerPtr->GetName());
}

void UBumperCarNeutralState::OnFireTrailOverlap(AActor* overlappingActor)
{
	if (overlappingActor == OwnerPtr)
	{
		bOnFire = true;
	}
}

void UBumperCarNeutralState::OnSlimeTrailOverlap(AActor* overlappingActor)
{
	if (overlappingActor == OwnerPtr)
	{
		bIsSlimed = true;
	}

	GEngine->AddOnScreenDebugMessage(3, 2.f, FColor::Green, "SLIMED");
}

void UBumperCarNeutralState::SetupTrailEvents()
{
	for (TObjectIterator<UTrailCollisionComponent> Iterator; Iterator; ++Iterator)
	{
		UTrailCollisionComponent* TrailCollisionComponentPtr = *Iterator;
		if (TrailCollisionComponentPtr)
		{
			TrailCollisionComponentPtr->OnFireTrailOverlapEvent.AddDynamic(this, &UBumperCarNeutralState::OnFireTrailOverlap);
			TrailCollisionComponentPtr->OnSlimeTrailOverlapEvent.AddDynamic(this, &UBumperCarNeutralState::OnSlimeTrailOverlap);
		}
	}

	bTrailEventsSetup = true;
}

void UBumperCarNeutralState::CalculateStartHealingTime(float deltaTime)
{
	if (TimerStarted)
	{
		AccumulatedStartHealingTime += deltaTime;
		if (AccumulatedStartHealingTime >= StartHealingTime)
		{
			ABumperCar* car = Cast<ABumperCar>(OwnerPtr);
			car->GetHealthComponent()->SetCanHeal(true);
			TimerStarted = false;
		}
	}
}

