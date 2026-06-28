// Copyright Abonneer, Inc. All Rights Reserved.


#include "BumperCarState.h"
#include "Net/UnrealNetwork.h"

void UBumperCarState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UBumperCarState, StateType);
}