// Copyright Abonneer, Inc. All Rights Reserved.


#include "MainPlayerController.h"
#include "StarterProject/States/LobbyGameState.h"
#include "StarterProject/GameModes/LobbyGameMode.h"
#include "StarterProject/States/GameplayGameState.h"
#include "Kismet/GameplayStatics.h"
#include "StarterProject/GameModes/GameplayGameMode.h"

#include "GameFramework/GameStateBase.h"
AMainPlayerController::AMainPlayerController()
{
   LobbyComponent = CreateDefaultSubobject<ULobbyControllerComponent>(TEXT("LobbyComponent"));
   LobbyComponent->SetAutoActivate(false);

   GameComponent = CreateDefaultSubobject<UGameControllerComponent>(TEXT("GameComponent"));
   GameComponent->SetAutoActivate(false);

   LoadingComponent = CreateDefaultSubobject<ULoadingControllerComponent>(TEXT("LoadingComponent"));
   LoadingComponent->SetAutoActivate(false);
}

void AMainPlayerController::BeginPlay()
{
    Super::BeginPlay();   

    UE_LOG(LogTemp, Log, TEXT("On BeginPlay called"));

    AGameStateBase* GameState = GetWorld()->GetGameState();

    if (IsLocalController())
    {
        if (Cast<ALobbyGameState>(GameState))
        {
            SwitchToLobby();
        }
        if (Cast<AGameplayGameState>(GameState))
        {
            SwitchToGame();
        }
    }
}

void AMainPlayerController::SwitchToLobby()
{
    if (GameComponent)
    {
        GameComponent->Deactivate();
    }

    if (LobbyComponent)
    {
        LobbyComponent->Activate(true);
        LobbyComponent->SetupLobbyUI();
        LobbyComponent->SetupLobbyInput();
    }
}

void AMainPlayerController::SwitchToGame()
{
    if (LobbyComponent)
    {
        LobbyComponent->Deactivate();
    }

    if (GameComponent)
    {
        GameComponent->Activate(true);
        GameComponent->SetUpComponent();
        GameComponent->Client_DisableGameInput();
    }

    OnSwitchToGame();
}

void AMainPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);


    //switching to game from game state
    /*if (Cast<AGameplayGameState>(GetWorld()->GetGameState()))
    {
        SwitchToGame();
    }*/
}

void AMainPlayerController::ShowLoadingScreen()
{
    if (IsLocalController() && LoadingComponent)
    {
        LoadingComponent->ShowLoadingScreen();
    }
}

void AMainPlayerController::Client_ShowLoadingScreen_Implementation()
{
    ShowLoadingScreen();
}

void AMainPlayerController::Client_HideLoadingScreen_Implementation()
{
    if (IsLocalController() && LoadingComponent)
    {
        LoadingComponent->RequestHideLoadingScreen();
    }
}


void AMainPlayerController::StartCheckingMapReady()
{
    // Check readiness
    if (!CheckIfMapLoaded())
    {
        // Not ready yet, check again next frame
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, this, &AMainPlayerController::StartCheckingMapReady, 0.1f, false);
    }
}

bool AMainPlayerController::CheckIfMapLoaded()
{
    if (!IsLocalController()) return false;

    UWorld* World = GetWorld();
    if (!World) return false;

    // Check async loading
    bool bAsyncDone = !IsAsyncLoading();

    // Check texture streaming completion
    bool bTexturesLoaded = true;
    auto& StreamingManager = IStreamingManager::Get().GetRenderAssetStreamingManager();

    // Loop through all levels
    ULevel* CurrentLevel = World->PersistentLevel;
    if (!CurrentLevel) return false;

    for (UObject* Obj : CurrentLevel->Actors)
    {
        if (!Obj) continue;

        UTexture* Texture = Cast<UTexture>(Obj);
        if (Texture && !StreamingManager.IsFullyStreamedIn(Texture))
        {
            bTexturesLoaded = false;
            break;
        }

        if (!bTexturesLoaded) break;
    }
    
    if (bAsyncDone && bTexturesLoaded)
    {
        //Server_NotifyClientReady();
        return true;
    }

    return false;
}

void AMainPlayerController::Server_NotifyClientReady_Implementation()
{
    if (AGameplayGameMode* GM = Cast<AGameplayGameMode>(GetWorld()->GetAuthGameMode()))
    {
        UE_LOG(LogTemp, Log, TEXT("Notify client ready: %s"), *GetOwner()->GetName());
        GM->ClientReady(this);
    }
}

void AMainPlayerController::Client_ShowPlayerDiedWidget_Implementation(const FString& deadPlayerName)
{
    ShowPlayerDiedWidget(deadPlayerName); // Calls your Blueprint event ON THE CLIENT
}