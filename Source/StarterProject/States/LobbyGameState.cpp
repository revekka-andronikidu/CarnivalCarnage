// Copyright Abonneer, Inc. All Rights Reserved.


#include "LobbyGameState.h"
#include "StarterProject/PlayerController/MainPlayerController.h"
#include <Kismet/GameplayStatics.h>

void ALobbyGameState::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    if (!World) return;

    // Iterate through ALL PlayerControllers in the world
    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (!PC) continue;

        if (PC->IsLocalController())
        {
            if (AMainPlayerController* MyPC = Cast<AMainPlayerController>(PC))
            {
                // Call your custom function
                MyPC->SwitchToLobby();
            }
        }
    }
}

void ALobbyGameState::MulticastOnPlayerJoined_Implementation(bool bJoined)
{
    OnPlayerJoined(bJoined); 
}

void ALobbyGameState::OnPlayerJoined(bool bJoined)
{
    if (!JoinSound || !LeftSound) return;

    if(bJoined)
    UGameplayStatics::PlaySound2D(GetWorld(), JoinSound);
    else
        UGameplayStatics::PlaySound2D(GetWorld(), LeftSound);
}