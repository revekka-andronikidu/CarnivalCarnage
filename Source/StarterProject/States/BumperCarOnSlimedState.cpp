// Copyright Abonneer, Inc. All Rights Reserved.


#include "BumperCarOnSlimedState.h"
#include "BumperCarGhostState.h"
#include "BumperCarOnFireState.h"
#include "BumperCarNeutralState.h"
#include "StarterProject/DeathBox.h"
#include "StarterProject/BumperCar.h"
#include "StarterProject/HealthComponent.h"
#include "StarterProject/TrailCollisionComponent.h"

TObjectPtr<UBumperCarState> UBumperCarOnSlimedState::HandleState(float deltaTime, AActor* ownerPtr)
{
	if (!bTrailEventsSetup) { SetupTrailEvent(); }

	if (OwnerPtr->GetHealthComponent()->GetIsDead()) 
	{ 
		return NewObject<UBumperCarGhostState>(); 
	}

	if (bOnFire) { return NewObject<UBumperCarOnFireState>(); }

	SlimedTimeElapsed += deltaTime;
	if (SlimedTimeElapsed >= TotalSlimedTime)
	{
		SlimedTimeElapsed = 0;
		return NewObject<UBumperCarNeutralState>();
	}

	
	return nullptr;
}

void UBumperCarOnSlimedState::OnEnter(AActor* ownerPtr)
{
	if (auto car = Cast<ABumperCar>(ownerPtr))
	{
		OwnerPtr = car;
	}
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("SLIMED STATE"));


	OwnerPtr->Client_IsSlimed(true);
}

void UBumperCarOnSlimedState::OnExit(AActor* ownerPtr)
{
	OwnerPtr->Client_IsSlimed(false);
}

void UBumperCarOnSlimedState::OnFireTrailOverlap(AActor* overlappingActor)
{
	if (overlappingActor == OwnerPtr)
	{
		bOnFire = true;
	}
}

void UBumperCarOnSlimedState::SetupTrailEvent()
{
	for (TObjectIterator<UTrailCollisionComponent> Iterator; Iterator; ++Iterator)
	{
		UTrailCollisionComponent* TrailCollisionComponentPtr = *Iterator;
		if (TrailCollisionComponentPtr)
		{
			TrailCollisionComponentPtr->OnFireTrailOverlapEvent.AddDynamic(this, &UBumperCarOnSlimedState::OnFireTrailOverlap);
		}
	}

	bTrailEventsSetup = true;
}