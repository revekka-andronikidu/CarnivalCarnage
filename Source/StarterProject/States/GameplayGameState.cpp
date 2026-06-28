// Copyright Abonneer, Inc. All Rights Reserved.


#include "GameplayGameState.h"
#include "Net/UnrealNetwork.h"
#include "StarterProject/PlayerController/MainPlayerController.h"

void AGameplayGameState::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    if (!World) return;

    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (!PC) continue;

        if (PC->IsLocalController())
        {
            if (AMainPlayerController* MyPC = Cast<AMainPlayerController>(PC))
                    {
                        // Call your custom function
                        MyPC->SwitchToGame();
                    }
        }
    }   
}


void AGameplayGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AGameplayGameState, CountdownStartTime);
    DOREPLIFETIME(AGameplayGameState, bRoundInProgress);
}

void AGameplayGameState::OnRep_CountdownStart()
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (AMainPlayerController* MPC = Cast<AMainPlayerController>(*It))
        {
            if (MPC->IsLocalController() && MPC->GameComponent)
            {
                MPC->GameComponent->OnCountdownReady();
            }
        }
    }
}

void AGameplayGameState::SetCountdownStartTime(float time)
{
    CountdownStartTime = time;

    if (HasAuthority())
    {
        // Call the RepNotify manually on the server
        OnRep_CountdownStart();
    }
}

void AGameplayGameState::OnRep_RoundInProgress()
{

}
