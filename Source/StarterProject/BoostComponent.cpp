// Copyright Abonneer, Inc. All Rights Reserved.


#include "BoostComponent.h"
#include "BumperCar.h"
#include "BumperCarMovementComponent.h"

DEFINE_LOG_CATEGORY(LogBoost);
// Sets default values for this component's properties
UBoostComponent::UBoostComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBoostComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	ABumperCar* Owner = Cast<ABumperCar>(GetOwner());
	if (Owner)
	{
		OwnerMovementComponentPtr = Owner->GetBumperCarMovementComponent();
		NetMulticast_PoleChargeEffect();
	}
}


// Called every frame
void UBoostComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (bIsAbsorbingBoost)
	{
		CurrentBoostAmount = FMath::Clamp(CurrentBoostAmount + BoostAbsorptionRate * GetWorld()->GetDeltaSeconds(), 0.f, MaxBoostAmount);
		NetMulticast_PoleChargeEffect();
	}
}

void UBoostComponent::ResetComponent()
{
	CurrentBoostAmount = 0.f;
	bIsAbsorbingBoost = false;
	NetMulticast_PoleChargeEffect();

	ABumperCar* Owner = Cast<ABumperCar>(GetOwner());
	Owner->OnBoostActiveEvent(false);
}

void UBoostComponent::Boost(bool isCompleted)
{
	if (CurrentBoostAmount <= 0.f) { return; }

	if (isCompleted) 
	{ 
		OwnerMovementComponentPtr->Boost(isCompleted);
		NetMulticast_PoleChargeEffect();
		return;
	}

	ABumperCar* Owner = Cast<ABumperCar>(GetOwner());
	CurrentBoostAmount = FMath::Clamp(CurrentBoostAmount - BoostConsumptionRate * GetWorld()->GetDeltaSeconds(), 0.f, MaxBoostAmount);
	if (CurrentBoostAmount <= 0.f)
	{
		
		Owner->OnBoostActiveEvent(false);
		OwnerMovementComponentPtr->Boost(true);
	}
	else
	{
		
		Owner->OnBoostActiveEvent(true);
		OwnerMovementComponentPtr->Boost(isCompleted);
	}

	// show boost on stick of car
	NetMulticast_PoleChargeEffect();
}

void UBoostComponent::Multicast_EmptyBoost_Implementation()
{
	CurrentBoostAmount = 0;
	NetMulticast_PoleChargeEffect();
}

void UBoostComponent::Multicast_AbsorbBoost_Implementation(bool isActive)
{
	if (bIsAbsorbingBoost != isActive) { bIsAbsorbingBoost = isActive; }
}

void UBoostComponent::NetMulticast_PoleChargeEffect_Implementation()
{
	if (ABumperCar* Owner = Cast<ABumperCar>(GetOwner()))
	{
		Owner->OnChargedPoleEvent(CurrentBoostAmount, true);
	}
}
