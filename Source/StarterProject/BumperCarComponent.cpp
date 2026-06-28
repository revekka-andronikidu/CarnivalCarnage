// Copyright Abonneer, Inc. All Rights Reserved.


#include "BumperCarComponent.h"
#include "States/BumperCarState.h"
#include "Net/UnrealNetwork.h"
#include "States/BumperCarNeutralState.h"

// Sets default values for this component's properties
UBumperCarComponent::UBumperCarComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UBumperCarComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)   // run only on server
	{
		StatePtr = NewObject<UBumperCarNeutralState>(this);
		StatePtr->OnEnter(GetOwner());

	}
}

void UBumperCarComponent::ResetComponent()
{
	if (GetOwnerRole() == ROLE_Authority)   // run only on server
	{
		SetState(UBumperCarNeutralState::StaticClass());
	}
}

void UBumperCarComponent::SetState(TSubclassOf<UBumperCarState> NewStateClass)
{
	if (GetOwnerRole() != ROLE_Authority)
		return; // States only exist on server

	if (!*NewStateClass)
		return;

	// Exit and destroy old state
	if (StatePtr)
	{
		StatePtr->OnExit(GetOwner());
		//StatePtr->ConditionalBeginDestroy();
		StatePtr = nullptr;
	}

	// Create new state
	StatePtr = NewObject<UBumperCarState>(this, NewStateClass);

	// Enter new state
	if (StatePtr)
	{
		StatePtr->OnEnter(GetOwner());
	}

	UE_LOG(LogTemp, Log, TEXT("State changed to %s"), *NewStateClass->GetName());
}



// Called every frame
void UBumperCarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwnerRole() == ROLE_Authority)   // run only on server
	{
		HandleState(DeltaTime);
	}
}

void UBumperCarComponent::HandleState(float deltaTime)
{
	auto newState = StatePtr->HandleState(deltaTime, GetOwner());
	if (newState)
	{
		StatePtr->OnExit(GetOwner());
		StatePtr = MoveTemp(newState);
		StatePtr->OnEnter(GetOwner());

		StateType = StatePtr->GetType();
	}
}

EBumperCarStateType UBumperCarComponent::GetStateType() const
{ 
	return StateType;
};


void UBumperCarComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBumperCarComponent, StateType);
}