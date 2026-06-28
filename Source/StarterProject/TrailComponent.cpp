// Copyright Abonneer, Inc. All Rights Reserved.


#include "TrailComponent.h"
#include "Components/SplineComponent.h"
#include "Trail.h"
#include "BumperCarMovementComponent.h"

// Sets default values for this component's properties
UTrailComponent::UTrailComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTrailComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UTrailComponent::ResetComponent()
{
	CurrentSpawnFrequencyTimer = 0.f;
	//PointSpawnFrequency = 0.1f; //no need to reset these? their value never changed
	//MaxPointLifeTime = 1.5f;
	SplinePointLifeTime.Empty();
}


// Called every frame
void UTrailComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner()->GetOwner()) { return; }

	if (!MovementComponentPtr)
	{
		MovementComponentPtr = GetOwner()->GetOwner()->FindComponentByClass<UBumperCarMovementComponent>();
	}

	CheckIfPointNeedsToSpawn(DeltaTime);
	CheckIfPointIsDead(DeltaTime);
}

void UTrailComponent::DeleteTimer(int index)
{
	if (SplinePointLifeTime.IsValidIndex(index))
	{
		SplinePointLifeTime.RemoveAt(index);
	}
}

void UTrailComponent::CheckIfPointNeedsToSpawn(float deltaTime)
{
	if (!MovementComponentPtr)
	{
		MovementComponentPtr = GetOwner()->GetOwner()->FindComponentByClass<UBumperCarMovementComponent>();
		if (!MovementComponentPtr)
		{
			UE_LOG(LogTrailActor, Warning, TEXT("MovementComponentPtr is NULL on %s (AUTHORITY=%d)"), *GetOwner()->GetName(), GetOwner()->HasAuthority());
			return;
		}
	}
	
	if (MovementComponentPtr->GetCurrentSpeed() <= 0.f) { return; }

	ATrail* trail = Cast<ATrail>(GetOwner());
	if (trail)
	{
		if (trail->GetOwnerIsHit()) { return; }
	}

	CurrentSpawnFrequencyTimer += deltaTime;
	if (CurrentSpawnFrequencyTimer >= PointSpawnFrequency)
	{
		CurrentSpawnFrequencyTimer -= PointSpawnFrequency;
		if (trail)
		{
			trail->SpawnPoint();
			SplinePointLifeTime.Add(0.f);
		}
	}
}

void UTrailComponent::CheckIfPointIsDead(float deltaTime)
{
	for (int index{}; index < SplinePointLifeTime.Num(); ++index)
	{
		SplinePointLifeTime[index] += deltaTime;
		if (SplinePointLifeTime[index] >= MaxPointLifeTime)
		{
			SplinePointLifeTime.RemoveAt(0);
			if (ATrail* trail = CastChecked<ATrail>(GetOwner()))
			{
				trail->DeletePoint();
			}
		}
	}
}
