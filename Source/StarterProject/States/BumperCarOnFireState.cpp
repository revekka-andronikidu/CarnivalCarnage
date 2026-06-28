// Copyright Abonneer, Inc. All Rights Reserved.

#include "BumperCarOnFireState.h"
#include "BumperCarNeutralState.h"
#include "BumperCarGhostState.h"
#include "StarterProject/BumperCar.h"
#include "StarterProject/BumperCarMovementComponent.h"
#include "StarterProject/HealthComponent.h"
#include "StarterProject/DeathBox.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

DEFINE_LOG_CATEGORY(LogOnFireState);

TObjectPtr<UBumperCarState> UBumperCarOnFireState::HandleState(float deltaTime, AActor* ownerPtr)
{	
	CalcDamage(deltaTime);
	if (CheckButtonPressTimer(deltaTime)) 
	{ 
		ResetButtonPresses(); 
	}
	
	if (ButtonPressCounter >= AmountOfPressesToExtinguish) 
	{ 
		UE_LOG(LogOnFireState, Log, TEXT("Neutral state called."));
		return NewObject<UBumperCarNeutralState>(); 
	}
	//else if (bIsDead) { return NewObject<UBumperCarGhostState>(); }
	return nullptr;
}

void UBumperCarOnFireState::OnEnter(AActor* ownerPtr)
{
	GEngine->AddOnScreenDebugMessage(3, 2.f, FColor::Green, TEXT("ON FIRE STATE"));
	StateType = EBumperCarStateType::OnFire;

	if (auto car = Cast<ABumperCar>(ownerPtr))
	{
		UE_LOG(LogOnFireState, Log, TEXT("%s entered on fire state."), *car->GetName());
		car->OnFireEvent(true);        // server sets authoritative state
		car->MulticastStartFire();
		car->GetHealthComponent()->SetCanHeal(false);
		
		//car->OnExtinguishPressEvent.AddDynamic(this, &UBumperCarOnFireState::ExtinguishButtonPressed);
		OwnerPtr = car;
	}
}

void UBumperCarOnFireState::OnExit(AActor* ownerPtr)
{
	
	if (auto car = Cast<ABumperCar>(ownerPtr))
	{
		UE_LOG(LogOnFireState, Log, TEXT("%s exited on fire state."), *car->GetName());
		car->OnFireEvent(false);        // server sets authoritative state
		car->MulticastStopFire();     // replicate to all clients
	}
	OwnerPtr->GetBumperCarMovementComponent()->DisableSteering(false);
}

//void UBumperCarOnFireState::ExtinguishButtonPressed()
//{
//	if (ButtonPressCounter >= AmountOfPressesToExtinguish) { return; }
//
//	++ButtonPressCounter;
//	DeltaTimeBetweenPresses = 0.f;
//	OwnerPtr->GetBumperCarMovementComponent()->DisableSteering(true);
//
//	UE_LOG(LogOnFireState, Log, TEXT("Extinguis pressed."));
//}

void UBumperCarOnFireState::ServerRegisterExtinguishPress_Implementation()
{
	if (ButtonPressCounter >= AmountOfPressesToExtinguish) { return; }

	++ButtonPressCounter;
	DeltaTimeBetweenPresses = 0.f;
	OwnerPtr->GetBumperCarMovementComponent()->DisableSteering(true);
	OwnerPtr->OnExtinguishPressedEvent();

	UE_LOG(LogOnFireState, Log, TEXT("Extinguis pressed."));
}

void UBumperCarOnFireState::ExtinguishButtonPressed()
{
	if (OwnerPtr->HasAuthority())
	{
		ServerRegisterExtinguishPress_Implementation();
		UE_LOG(LogOnFireState, Log, TEXT("Extinguish pressed (server)."));
	}
	else
	{
		ServerRegisterExtinguishPress();
		UE_LOG(LogOnFireState, Log, TEXT("Extinguish pressed (client)."));
	}	
}

bool UBumperCarOnFireState::CheckButtonPressTimer(float deltaTime)
{
	if (ButtonPressCounter <= 0) { return false; }

	DeltaTimeBetweenPresses += deltaTime;
	if (DeltaTimeBetweenPresses > MaxTimeBetweenPresses) { return true; }

	return false;
}

void UBumperCarOnFireState::ResetButtonPresses()
{
	ButtonPressCounter = 0;
	OwnerPtr->GetBumperCarMovementComponent()->DisableSteering(false);
}

void UBumperCarOnFireState::CalcDamage(float deltaTime)
{
	CurrentDamageTime += deltaTime;
	if (CurrentDamageTime >= 1.f / DamagePerSecond)
	{
		OwnerPtr->GetHealthComponent()->DoDamage(1);
		CurrentDamageTime -= 1.f / DamagePerSecond;
	}
}



