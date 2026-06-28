// Copyright Abonneer, Inc. All Rights Reserved.


#include "LobbyGameMode.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "StarterProject/PlayerController/MainPlayerController.h"
#include "StarterProject/Lobby/LobbyPlayerWindow.h"
#include "Kismet/GameplayStatics.h"
#include "StarterProject/PlayerController/MainPlayerState.h"
#include "Framework/GameInstances/CustomGameInstanceBase.h"
#include "StarterProject/States/LobbyGameState.h"
#include "Online/AdvancedSessions/AdvancedSessionsLibrary.h"
ALobbyGameMode::ALobbyGameMode()
{
    HostController = nullptr;
    PlayerStateClass = AMainPlayerState::StaticClass();

   
    static ConstructorHelpers::FClassFinder<ALobbyCharacterActor> ClownBlue(TEXT("/Game/Blueprints/Lobby/Characters/BP_ClownBlue.BP_ClownBlue_C"));
    static ConstructorHelpers::FClassFinder<ALobbyCharacterActor> ClownOrange(TEXT("/Game/Blueprints/Lobby/Characters/BP_ClownOrange.BP_ClownOrange_C"));
    static ConstructorHelpers::FClassFinder<ALobbyCharacterActor> ClownGreen(TEXT("/Game/Blueprints/Lobby/Characters/BP_ClownGreen.BP_ClownGreen_C"));
    static ConstructorHelpers::FClassFinder<ALobbyCharacterActor> ClownRed(TEXT("/Game/Blueprints/Lobby/Characters/BP_ClownRed.BP_ClownRed_C"));
    static ConstructorHelpers::FClassFinder<ALobbyCharacterActor> ClownPurple(TEXT("/Game/Blueprints/Lobby/Characters/BP_ClownPurple.BP_ClownPurple_C"));
    static ConstructorHelpers::FClassFinder<ALobbyCharacterActor> ClownYellow(TEXT("/Game/Blueprints/Lobby/Characters/BP_ClownYellow.BP_ClownYellow_C"));

  
    if (ClownBlue.Succeeded()) AllCharacterClasses.Add(ClownBlue.Class);
    if (ClownOrange.Succeeded()) AllCharacterClasses.Add(ClownOrange.Class);
    if (ClownGreen.Succeeded()) AllCharacterClasses.Add(ClownGreen.Class);
    if (ClownPurple.Succeeded()) AllCharacterClasses.Add(ClownPurple.Class);
    if (ClownRed.Succeeded()) AllCharacterClasses.Add(ClownRed.Class);
    if (ClownYellow.Succeeded()) AllCharacterClasses.Add(ClownYellow.Class);

    AvailableCharacters = AllCharacterClasses;
}

void ALobbyGameMode::BeginPlay()
{
    Super::BeginPlay();

    SetUpPlayerWindows();
    bUseSeamlessTravel = true;
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (!NewPlayer) return;

    LobbyPlayerControllers.Add(NewPlayer);
    ALobbyGameState* GS = GetGameState<ALobbyGameState>();
    if (GS)
    {
        GS->MulticastOnPlayerJoined(false);
    }

    if (!HostController && NewPlayer->HasAuthority())
    {
        HostController = NewPlayer;
        UE_LOG(LogTemp, Log, TEXT("HostController set to %s"), *NewPlayer->GetName());
    }

    APlayerState* BasePS = NewPlayer->GetPlayerState<APlayerState>();
    FString playerName = NewPlayer->GetPlayerState<APlayerState>()->GetPlayerName();
   

    UE_LOG(LogTemp, Log, TEXT("Player joined lobby: %s"), *playerName);
    UE_LOG(LogTemp, Log, TEXT("Total players in lobby: %d"), LobbyPlayerControllers.Num());

   
    AMainPlayerState* PS = Cast<AMainPlayerState>(BasePS);
    if (PS)
    {
        PS->SetSteamPlayerName(playerName);
        
        UE_LOG(LogTemp, Log, TEXT("Saved Steam Name to PlayerState: %s"), *playerName);
    }

    AssignCharacterToPlayer(NewPlayer);

    if (bSetupFinished)
    {
        UpdateWindows();
    }
    else
    {
        // If setup isn't finished, schedule a check to update posts when ready
        GetWorldTimerManager().SetTimerForNextTick(this, &ALobbyGameMode::CheckAndAssignNewPlayers);
    }
}

void ALobbyGameMode::StartGame()
{
    if (!HasAuthority())
        return;

    if (UCustomGameInstanceBase* GI = GetGameInstance<UCustomGameInstanceBase>())
    {
        GI->ExpectedPlayers = LobbyPlayerControllers.Num();
    }

    for (APlayerController* PC : LobbyPlayerControllers)
    {
        if (!PC) continue;
        AMainPlayerController* MainPC = Cast<AMainPlayerController>(PC);
        if (!MainPC) continue;

        ULobbyControllerComponent* LobbyComp = MainPC->FindComponentByClass<ULobbyControllerComponent>();
        if (!LobbyComp)
        {
            UE_LOG(LogTemp, Warning, TEXT("No LobbyControllerComponent found on %s"), *MainPC->GetName());
            continue;
        }
        LobbyComp->Client_OnGameStart();
    }

    ServerTravel();    
}

void ALobbyGameMode::ServerTravel()
{
    FString TravelURL = MatchMapPath + TEXT("?listen?seamless");
    GetWorld()->GetAuthGameMode()->bUseSeamlessTravel = true;
    GetWorld()->ServerTravel(TravelURL, true);
    //UE_LOG(LogTemp, Log, TEXT("ServerTravel to: %s"), *MatchMapPath);
}

void ALobbyGameMode::AssignCharacterToPlayer(APlayerController* NewPlayer)
{
    if (!NewPlayer) return;

    AMainPlayerState* PS = NewPlayer->GetPlayerState<AMainPlayerState>();
    if (PS)
    {
        if (AvailableCharacters.Num() > 0)
        {
            TSubclassOf<ALobbyCharacterActor> SelectedClass = AvailableCharacters[0];
            PS->SetSelectedCharacter(SelectedClass);
            AssignedCharacterMap.Add(NewPlayer, SelectedClass);
            AvailableCharacters.RemoveAt(0);
   
            UE_LOG(LogTemp, Log, TEXT("Assigned %s to %s"), *SelectedClass->GetName(), *NewPlayer->GetName());
        }
    }
}

void ALobbyGameMode::SetUpPlayerWindows()
{
    PlayerWindows.Empty();
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALobbyPlayerWindow::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        if (ALobbyPlayerWindow* LobbyWindow = Cast<ALobbyPlayerWindow>(Actor))
        {
            PlayerWindows.Add(LobbyWindow);
        }
    }
    bSetupFinished = true;

    UE_LOG(LogTemp, Log, TEXT("Setup finished. Total windows found: %d"), PlayerWindows.Num());
}

void ALobbyGameMode::UpdateWindows()
{
    for (APlayerController* PC : LobbyPlayerControllers)
    {
        bool bAlreadyAssigned = false;
        for (ALobbyPlayerWindow* Post : PlayerWindows)
        {
            if (Post->GetAssignedPC() == PC)
            {
                bAlreadyAssigned = true;
                break;
            }
        }
        if (bAlreadyAssigned)
            continue;

        for (ALobbyPlayerWindow* Post : PlayerWindows)
        {
            if (!Post->HasPlayerAssigned())
            {
                Post->SpawnCharacter(PC);
                UE_LOG(LogTemp, Log, TEXT("Assigned player %s to post %s"), *PC->GetName(), *Post->GetName());
                break;
            }
        }
    }
    RemoveDisconnectedPlayers();
}

void ALobbyGameMode::Logout(AController* ExitingController)
{
    APlayerController* ExitingPC = Cast<APlayerController>(ExitingController);
    if (!ExitingPC)
        return;

    ALobbyGameState* GS = GetGameState<ALobbyGameState>();
    if (GS)
    {
        GS->MulticastOnPlayerJoined(false);
    }



    LobbyPlayerControllers.Remove(ExitingPC);

    TSubclassOf<ALobbyCharacterActor> FreedCharacter = nullptr;
    if (AssignedCharacterMap.RemoveAndCopyValue(ExitingPC, FreedCharacter))
    {
        if (FreedCharacter && !AvailableCharacters.Contains(FreedCharacter))
        {
            AvailableCharacters.Add(FreedCharacter);
            UE_LOG(LogTemp, Log, TEXT("Freed character %s from disconnected player"), *FreedCharacter->GetName());
        }
    }

    if (bSetupFinished)
    {
        UpdateWindows();
    }
    else
    {
        GetWorldTimerManager().SetTimerForNextTick(this, &ALobbyGameMode::CheckAndAssignNewPlayers);
    }

    Super::Logout(ExitingController);
}

void ALobbyGameMode::RemoveDisconnectedPlayers()
{
    for (ALobbyPlayerWindow* Post : PlayerWindows)
    {
        if (!Post) continue;

        APlayerController* PC = Post->GetAssignedPC();

        if (!PC) continue;

        if (!LobbyPlayerControllers.Contains(PC))
        {
            Post->Clear();
            UE_LOG(LogTemp, Log, TEXT("Cleared post %s because player %s disconnected"), *Post->GetName(), *PC->GetName());
        }
    }
}

void ALobbyGameMode::CheckAndAssignNewPlayers()
{
    if (bSetupFinished)
    {
        UpdateWindows();
    }
    else
    {
        GetWorldTimerManager().SetTimerForNextTick(this, &ALobbyGameMode::CheckAndAssignNewPlayers);
    }
}

void ALobbyGameMode::OnReady(APlayerController* requesingPC, bool isReady)
{
    if (!requesingPC)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnReady called with null RequestingPC!"));
        return;
    }

    for (ALobbyPlayerWindow* Window : PlayerWindows)
    {
        if (!Window) continue;

        APlayerController* WindowPC = Window->GetPC();

        if (WindowPC == requesingPC)
        {
            UE_LOG(LogTemp, Log, TEXT("PlayerController %s found in window %s"),
                *requesingPC->GetName(), *Window->GetName());

            Window->SetReady(isReady);


            CheckAllPlayersReady();
            return;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("No window found for %s"), *requesingPC->GetName());
}

void ALobbyGameMode::CheckAllPlayersReady()
{
    bool bAllReady = true;
    int32 ReadyCount = 0;
    int32 TotalCount = LobbyPlayerControllers.Num();

    for (auto* PC : LobbyPlayerControllers)
    {
        if (!PC) continue;

        // Get the component
        ULobbyControllerComponent* LobbyComp = PC->FindComponentByClass<ULobbyControllerComponent>();
        if (LobbyComp && LobbyComp->GetIsReady())
        {
            ReadyCount++;
        }
        else
        {
            bAllReady = false;
        }
    }

    AMainPlayerController* MainPC = Cast<AMainPlayerController>(HostController);
    if (!MainPC) return;

    ULobbyControllerComponent* LobbyComp = MainPC->FindComponentByClass<ULobbyControllerComponent>();
    if (!LobbyComp) return;

    LobbyComp->EnableStartButton(bAllReady);
}


void ALobbyGameMode::SwitchCharacter(APlayerController* RequestingPC, TSubclassOf<ALobbyCharacterActor> NewCharacter)
{
    if (!RequestingPC || !NewCharacter) return;

    AMainPlayerState* PS = RequestingPC->GetPlayerState<AMainPlayerState>();
    if (!PS) return;

    // Check if the requested character is available
    if (AvailableCharacters.Contains(NewCharacter))
    {
        // Return current character to pool
        TSubclassOf<ALobbyCharacterActor> CurrentCharacter = AssignedCharacterMap.FindRef(RequestingPC);
        if (CurrentCharacter)
        {
            AvailableCharacters.Add(CurrentCharacter);
        }

        // Assign new character
        AssignedCharacterMap.Add(RequestingPC, NewCharacter);
        AvailableCharacters.Remove(NewCharacter);
        PS->SetSelectedCharacter(NewCharacter);

        UE_LOG(LogTemp, Log, TEXT("Player %s switched to character %s"), *RequestingPC->GetName(), *NewCharacter->GetName());

        // Update the visual representation
        UpdateWindows();
    }
}

void ALobbyGameMode::CycleCharacter(APlayerController* RequestingPC, bool bNext)
{
    if (!RequestingPC) return;

    AMainPlayerState* PS = RequestingPC->GetPlayerState<AMainPlayerState>();
    if (!PS) return;

    TSubclassOf<ALobbyCharacterActor> CurrentCharacter = AssignedCharacterMap.FindRef(RequestingPC);
    if (!CurrentCharacter) return;



    // Build a combined array: available + currently assigned characters
    TArray<TSubclassOf<ALobbyCharacterActor>> Characters;
    Characters.Append(AvailableCharacters);
    Characters.Add(CurrentCharacter);

    // Find current character index
    int32 Index = Characters.IndexOfByKey(CurrentCharacter);
    if (Index == INDEX_NONE) return;

    // Determine next index
    int32 NewIndex = bNext ? (Index + 1) % Characters.Num() : (Index - 1 + Characters.Num()) % Characters.Num();
    TSubclassOf<ALobbyCharacterActor> NewCharacter = Characters[NewIndex];

    // Only switch if the character is not the same
    if (NewCharacter != CurrentCharacter)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                5.0f,
                FColor::Green,
                FString::Printf(TEXT("New character is: %s"), *NewCharacter->GetName())
            );
        }

        SwitchCharacter(RequestingPC, NewCharacter);
    }
}