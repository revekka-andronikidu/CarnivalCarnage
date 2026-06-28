// Copyright Abonneer, Inc. All Rights Reserved.


#include "MainPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "StarterProject/Lobby/LobbyCharacterActor.h"
#include "StarterProject/BumperCar.h"
#include "ImageUtils.h"
#include "Online/AdvancedSteamSessions/AdvancedSteamFriendsLibrary.h"
#include "StarterProject/States/GameplayGameState.h"
void AMainPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMainPlayerState, SelectedCharacterClass);
    DOREPLIFETIME(AMainPlayerState, Color);
    DOREPLIFETIME(AMainPlayerState, numberOfWins);
    DOREPLIFETIME(AMainPlayerState, SteamPlayerName);
}

void AMainPlayerState::OnRep_RoundWon()
{
    if (ABumperCar* car = Cast<ABumperCar>(GetPawn()))
    {
        car->OnRoundWon(numberOfWins);
    }

    UE_LOG(LogTemp, Log, TEXT("NumberOfWins changed."));

    AGameplayGameState* GS = GetWorld()->GetGameState<AGameplayGameState>();
    if (!GS) return;

    GS->OnLeaderboardChanged.Broadcast();
}

void AMainPlayerState::SetSelectedCharacter(TSubclassOf<ALobbyCharacterActor> InClass)
{
    SelectedCharacterClass = InClass;
    OnRep_SelectedCharacterClass(); 
}

void AMainPlayerState::AddWin()
{
    if (HasAuthority())
    {
        ++numberOfWins;

        // Manually for server and OnRep will be called automatically for the clients
        OnRep_RoundWon();
    }
}

void AMainPlayerState::OnRep_SelectedCharacterClass()
{
    UE_LOG(LogTemp, Log, TEXT("SelectedCharacterClass replicated for %s"), *GetPlayerName());
    // You can notify UI or call your window to spawn here
}

void AMainPlayerState::SetColor(float color)
{
    Color = color;
    OnRep_CharacterColor(); 
}

void AMainPlayerState::OnRep_CharacterColor()
{
    UE_LOG(LogTemp, Log, TEXT("Color replicated for %s: %f"), *GetPlayerName(), Color);
}

void AMainPlayerState::SetSteamPlayerName(const FString& NewName)
{
    SteamPlayerName = NewName;
    OnRep_PlayerName(); // Apply on server
}

UTexture2D* AMainPlayerState::GetSteamAvatar() const
{
    FBPUniqueNetId BPNetId;
    BPNetId.SetUniqueNetId(GetUniqueId().GetUniqueNetId());
         
    EBlueprintAsyncResultSwitch Result;
    UTexture2D* AvatarTex = UAdvancedSteamFriendsLibrary::GetSteamFriendAvatar(BPNetId, Result, SteamAvatarSize::SteamAvatar_Small);

    if (Result == EBlueprintAsyncResultSwitch::OnSuccess)
    {
        return AvatarTex;
    }
   return nullptr;
}

void AMainPlayerState::SeamlessTravelTo(APlayerState* NewPlayerState)
{
    Super::SeamlessTravelTo(NewPlayerState);

    AMainPlayerState* NewPS = Cast<AMainPlayerState>(NewPlayerState);
    if (NewPS)
    {
        NewPS->Color = Color;
        NewPS->SelectedCharacterClass = SelectedCharacterClass;
        NewPS->numberOfWins = numberOfWins;
        NewPS->SteamPlayerName = SteamPlayerName;
    }
}


FLeaderboardEntry AMainPlayerState::GetLeaderboardEntry() const
{
    FLeaderboardEntry entry;

    entry.PlayerName = SteamPlayerName;
    entry.PlayerScore = numberOfWins;
    entry.Color = Color;
    entry.AvatarTexture = GetSteamAvatar();

    return entry;
}