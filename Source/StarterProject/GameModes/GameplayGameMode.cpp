// Copyright Abonneer, Inc. All Rights Reserved.


#include "GameplayGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Framework/GameInstances/CustomGameInstanceBase.h"
#include "StarterProject/PlayerController/MainPlayerController.h"
#include "StarterProject/BumperCar.h"
#include "StarterProject/HealthComponent.h"
#include "StarterProject/States/GameplayGameState.h"
#include "StarterProject/PlayerController/MainPlayerState.h"
#include "StarterProject/UI/CountdownWidget.h"
#include "StarterProject/BoostNet.h"
#include "Engine/OverlapResult.h"


int32 NextPlayerStartIndex = 0;

DEFINE_LOG_CATEGORY(LogGameplayGameMode);
AActor* AGameplayGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

    if (PlayerStarts.Num() == 0)
    {
        // No PlayerStarts found, fallback to base class
        return Super::ChoosePlayerStart(Player);
    }


    int32 PlayerIndex = ArrivedPlayers.IndexOfByKey(Cast<APlayerController>(Player));

    if (PlayerIndex == INDEX_NONE)
    {
        PlayerIndex = NextPlayerStartIndex;
        NextPlayerStartIndex++;

        UE_LOG(LogGameplayGameMode, Log, TEXT("Player index by key not found"));
    }

    // 3. Clamp index to available PlayerStarts to avoid out-of-bounds
    AActor* ChosenStart = PlayerStarts[PlayerIndex % PlayerStarts.Num()];
    //int32 StartIndex = PlayerIndex % PlayerStarts.Num();

    // 4. Return the corresponding PlayerStart
    UE_LOG(LogGameplayGameMode, Log, TEXT("Chosen player start index: %d"), PlayerIndex);
    return ChosenStart;
}

void AGameplayGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
    Super::HandleSeamlessTravelPlayer(C);

    UE_LOG(LogGameplayGameMode, Log, TEXT("%s finished seamless travel"), *C->GetName());


    APlayerController* PC = Cast<APlayerController>(C);
    if (PC && !ArrivedPlayers.Contains(PC))
    {
        ArrivedPlayers.Add(PC);
        CheckAllPlayersArrived();
    }
}

void AGameplayGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    //make a spectator?

}

void AGameplayGameMode::Logout(AController* ExitingController)
{
   


    APlayerController* PC = Cast<APlayerController>(ExitingController);
    if (PC && !ArrivedPlayers.Contains(PC))
    {
        ArrivedPlayers.Remove(PC);
        UE_LOG(LogGameplayGameMode, Log, TEXT("Removed player: %s!"), *ExitingController->GetName());
    }

   /* if (ArrivedPlayers.Num() < 2)
    {
    //end match
    }*/

    Super::Logout(ExitingController);
}

void AGameplayGameMode::CheckAllPlayersArrived()
{
    int32 expectedPlayers = 0;
    if (UCustomGameInstanceBase* GI = GetGameInstance<UCustomGameInstanceBase>())
    {
        expectedPlayers = GI->ExpectedPlayers;
    }

    if (ArrivedPlayers.Num() >= expectedPlayers && expectedPlayers > 0)
    {
        UE_LOG(LogGameplayGameMode, Log, TEXT("All %d players have joined the game!"), expectedPlayers);
        OnAllPlayersArrived();
    }
}


void AGameplayGameMode::OnAllPlayersArrived()
{
    SetCarColor();

    //debug here, will actually have to check with loaded level
    //instead OnLeavelReady being called directly the client ready should be called by controller when level is fully loaded


    OnLevelLoaded();
}

void AGameplayGameMode::ClientReady(APlayerController* PC)
{
    ReadyPlayers.Add(PC);

    if (ReadyPlayers.Num() == ArrivedPlayers.Num())
    {
        UE_LOG(LogGameplayGameMode, Log, TEXT("All clients are fully ready!"));
        OnLevelLoaded();
    }
}

void AGameplayGameMode::OnLevelLoaded()
{
    for (auto PC : ArrivedPlayers)
    {
        AMainPlayerController* mainPC = Cast<AMainPlayerController>(PC);
        if (mainPC)
        {
            mainPC->Client_HideLoadingScreen();
            //mainPC->GameComponent->Client_OnEnableGravity();
        }
    }


    OnPlayStartSequence();
   
   //StartCountdown();
}


void AGameplayGameMode::OnStartSequenceFinished()
{
    StartCountdown();
}

void AGameplayGameMode::StartCountdown()
{
    auto GS = GetGameState<AGameplayGameState>();
    if (GS)
    GS->SetCountdownStartTime( GetWorld()->GetTimeSeconds());
    ReadyPlayers.Empty();
}

void AGameplayGameMode::NotifyClientFinishedCountdown(APlayerController* PC)
{
    if (!HasAuthority())
        return;

    if (!PC)
        return;

    // Add player to the ready list
    ReadyPlayers.Add(PC);

    // Check if all players are ready
    if (ReadyPlayers.Num() == ArrivedPlayers.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("All players are ready. Starting match..."));
        StartGameMatch();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("%d/%d players ready."),
            ReadyPlayers.Num(), GetNumPlayers());
    }
}

void AGameplayGameMode::StartGameMatch()
{
    AlivePlayers.Empty();
    DeadPlayers.Empty();
    bRoundResetInProgress = false;
    GetGameState<AGameplayGameState>()->bRoundInProgress = true;
    bRoundInProgress = true;

    UE_LOG(LogGameplayGameMode, Log, TEXT("Match Started!"));
    //enable player input and start gameplay
    for (APlayerController* PC : ArrivedPlayers)
    {
        if (ABumperCar* Car = Cast<ABumperCar>(PC->GetPawn()))
        {
            AlivePlayers.Add(Car);
            Car->SetActorEnableCollision(true);
        }

        AMainPlayerController* MyPC = Cast<AMainPlayerController>(PC);
        if (MyPC)
        {
            MyPC->GameComponent->Client_OnMatchStart();
        }
    }
   

}

void AGameplayGameMode::KillPlayer(ABumperCar* DeadActor)
{
    if (!HasAuthority()) return;

    if (!DeadActor) return;

    if (!bRoundInProgress)
    {
        UE_LOG(LogGameplayGameMode, Warning, TEXT("Round not in progress — skipping OnPlayerDied/respawning at start."));
        NewRoundRespawnCar(DeadActor);
        return;
    }

    AlivePlayers.Remove(DeadActor);
    DeadActor->KillPlayer();
    DeadPlayers.Add(DeadActor);

    

    UE_LOG(LogGameplayGameMode, Log, TEXT("Player %s died! Now %d alive, %d dead"), *DeadActor->GetName(), AlivePlayers.Num(), DeadPlayers.Num());

    //DEBUG
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            5.f,
            FColor::Yellow,
            FString::Printf(TEXT("Alive count: %d | Dead count: %d"), AlivePlayers.Num(), DeadPlayers.Num())
        );
    }

    OnPlayerDied(DeadActor);
}
void AGameplayGameMode::OnPlayerDied(ABumperCar* DeadActor)
{
	ShootCannonOnDeath();

    if (AlivePlayers.Num() == 1)
    {
       // DeadActor->Client_OnDeathBoxDeath();
        DeadActor->Client_DeathEffect();


        TArray<AActor*> boostNets;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoostNet::StaticClass(), boostNets);
        for (AActor* actor : boostNets)
        {
            auto* boostNet = Cast<ABoostNet>(actor);
            boostNet->Multicast_SetActive(false);
        }


        ABumperCar* Winner = AlivePlayers[0];

        EndRound(Winner);
        return;
    }
    else if (AlivePlayers.Num() == 0) // If zero alive (edge case: all died simultaneously)
    {
        //DeadActor->Client_OnDeathBoxDeath();
      

        UE_LOG(LogGameplayGameMode, Warning, TEXT("All players are dead! No winner this round."));
		EndRound(nullptr);
        return;
    }
    else
    {
        DisplayPlayeDiedWidget(DeadActor);
        DeadActor->Client_OnDeathBoxDeath();

        UE_LOG(LogGameplayGameMode, Warning, TEXT("Player died, game still in progress."));
        return;
    }
}


void AGameplayGameMode::EndRound(ABumperCar* winnerPtr)
{
    UE_LOG(LogGameplayGameMode, Warning, TEXT("== EndRound START (Round %d) =="), RoundNumber);

    GetGameState<AGameplayGameState>()->bRoundInProgress = false;
	bRoundInProgress = false;
    bRoundResetInProgress = true;


    if (winnerPtr)
    {
        AMainPlayerState* playerState = Cast<AMainPlayerState>(winnerPtr->GetPlayerState());
        if (playerState)
        {
            UE_LOG(LogGameplayGameMode, Warning, TEXT("Winner is: %s, incremented win in player state."), *winnerPtr->GetName());
            playerState->AddWin();

            if (playerState->GetNumberOfWins() >= 3)
            {
                UE_LOG(LogGameplayGameMode, Warning, TEXT("Player %s has won the match!"), *winnerPtr->GetName());

                OnMatchOver();
                return;  // Stop EndRound() — match is over
            }
        }
        else
        {
            UE_LOG(LogGameplayGameMode, Error, TEXT("Winner's PlayerState is NULL!"));
        }
    }
    else
    {
        UE_LOG(LogGameplayGameMode, Error, TEXT("Winner is NULL!"));
    }    

    for(APlayerController* PC : ArrivedPlayers)
    {
        AMainPlayerController* MyPC = Cast<AMainPlayerController>(PC);
        if (MyPC)
        {
            MyPC->GameComponent->Client_DisableGameInput();
        }
	}

    OnEndRoundEvent();
}

void AGameplayGameMode::RespawnAllCars()
{
    UE_LOG(LogGameplayGameMode, Log, TEXT(" On Round finished called (GM)"));

    for (auto playerController : ArrivedPlayers)
    {
        if (ABumperCar* car = Cast<ABumperCar>(playerController->GetPawn()))
        {
            UE_LOG(LogGameplayGameMode, Log, TEXT("Resetting and respawning car: %s (GM)"), *car->GetName());
           

            NewRoundRespawnCar(car);
            //car->Multicast_ResetBumperCar();
        }
    }
}

void AGameplayGameMode::StartNextRound()
{
    ++RoundNumber;
    UE_LOG(LogGameplayGameMode, Log, TEXT("Round incremented to %d — starting next round."), RoundNumber);

    TArray<AActor*> boostNets;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoostNet::StaticClass(), boostNets);
    for (AActor* actor : boostNets)
    {
        auto* boostNet = Cast<ABoostNet>(actor);
        boostNet->Multicast_SetActive(true);
    }

    StartCountdown();
}



void AGameplayGameMode::SetCarColor()
{
    for (APlayerController* PC : ArrivedPlayers)
    {
        if (ABumperCar* car = Cast<ABumperCar>(PC->GetPawn()))
        {
            AMainPlayerState* PS = PC->GetPlayerState<AMainPlayerState>();
            if (!PS) return;

            car->SetCarColor(PS->GetColor());
            UE_LOG(LogGameplayGameMode, Log, TEXT("Color set for %s: %f"), *car->GetName(), PS->GetColor());
        }
    }
}


void AGameplayGameMode::OnMatchOver()
{

    //play some animations


    //delay the transition


    //enable loading screen


    UE_LOG(LogGameplayGameMode, Log, TEXT("Transitioning to the end screen!"));
    TravelToEndScreen();
}

void AGameplayGameMode::TravelToEndScreen()
{
    for (APlayerController* PC : ArrivedPlayers)
    {
        if (!PC) continue;
        AMainPlayerController* MainPC = Cast<AMainPlayerController>(PC);
        if (!MainPC) continue;

        MainPC->Client_ShowLoadingScreen();
    }

    FString TravelURL = EndScreenMapPath + TEXT("?listen?seamless");
    GetWorld()->GetAuthGameMode()->bUseSeamlessTravel = true;
    GetWorld()->ServerTravel(TravelURL, true);
}

void  AGameplayGameMode::RespawnAsGhost(ABumperCar* DeadActor)
{
    if (!DeadActor) return;
    if (!bRoundInProgress) return;

    if (DeadActor->GetStateType() != EBumperCarStateType::Ghost)
    {
        DeadActor->ChangeToGhostState();
    }

   
    RespawnCar(DeadActor);
    DeadActor->SetActorEnableCollision(true);

    UE_LOG(LogGameplayGameMode, Log, TEXT("Player %s respawn as ghost."), *DeadActor->GetName());
}

void AGameplayGameMode::RespawnCar(ABumperCar* carToRespawn)
{
    AController* Controller = carToRespawn->GetController();
    if (!Controller) return;

    UE_LOG(LogGameplayGameMode, Log, TEXT("Car %s is being respawn."), *carToRespawn->GetName());

    carToRespawn->Multicast_RespawnBumperCar(ChoosePlayerStart(Controller)->GetActorTransform());
}

void AGameplayGameMode::NewRoundRespawnCar(ABumperCar* carToRespawn)
{
    AController* Controller = carToRespawn->GetController();
    if (!Controller) return;

    UE_LOG(LogGameplayGameMode, Log, TEXT("Car %s is being respawn."), *carToRespawn->GetName());

    //carToRespawn->Client_OnRespawn();
    carToRespawn->Multicast_NewRoundRespawnBumperCar(ChoosePlayerStart(Controller)->GetActorTransform());
    carToRespawn->Multicast_ResetBumperCar();
}


void AGameplayGameMode::DisplayPlayeDiedWidget(ABumperCar* DeadActor)
{
    UE_LOG(LogGameplayGameMode, Warning, TEXT("Display dead widget called for %s"), *DeadActor->GetName());

    AController* DeadController = nullptr;

    if (APawn* DeadPawn = Cast<APawn>(DeadActor))
    {
        DeadController = DeadPawn->GetController();
    }

    for (APlayerController* PC : ArrivedPlayers)
    {
        AMainPlayerController* MainPC = Cast<AMainPlayerController>(PC);
        if (!MainPC || MainPC == DeadController) continue;

        // Call the function
        UE_LOG(LogGameplayGameMode, Warning, TEXT("Display dead SHOW called."));


        AMainPlayerState* playerState = Cast<AMainPlayerState>(DeadActor->GetPlayerState());
        if (!playerState)
        {
            UE_LOG(LogGameplayGameMode, Error, TEXT("PlayerState cast failed for %s"), *MainPC->GetName());
            continue;
        }

        MainPC->Client_ShowPlayerDiedWidget(playerState->GetSteamPlayerName());
    }

}

void AGameplayGameMode::RequestedRespawn(ABumperCar* carToRespawn)
{
    UE_LOG(LogGameplayGameMode, Log, TEXT("Respawn called for: %s."), *carToRespawn->GetName());

    if (!HasAuthority())
        return;

    if (!bRoundInProgress)
    {
        carToRespawn->Client_FadeIn();
            return;
    }
        
    //check for conditions, round end? is ghost?
    // 
    //play respawn effect (call BP event)

    carToRespawn->IsRespawning = true;
    carToRespawn->Client_OnRespawn();


    RespawnAsGhost(carToRespawn);
}