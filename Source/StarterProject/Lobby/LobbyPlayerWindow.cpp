// Copyright Abonneer, Inc. All Rights Reserved.


#include "LobbyPlayerWindow.h"
#include "Components/ArrowComponent.h"
#include "ReadyDisplay.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"
#include "StarterProject/PlayerController/MainPlayerState.h"
#include "StarterProject/PlayerController/MainPlayerController.h"
#include "StarterProject/PlayerController/LobbyControllerComponent.h"


// Sets default values
ALobbyPlayerWindow::ALobbyPlayerWindow()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

}

// Called when the game starts or when spawned
void ALobbyPlayerWindow::BeginPlay()
{
    Super::BeginPlay();

    if (!PlayerSpawnArrow)
    {
        TArray<UArrowComponent*> Arrows;
        GetComponents<UArrowComponent>(Arrows);
        for (UArrowComponent* Arrow : Arrows)
        {
            if (Arrow && Arrow->GetName().Contains(TEXT("PlayerSpawnTransform")))
            {
                PlayerSpawnArrow = Arrow;
                UE_LOG(LogTemp, Log, TEXT("%s: Auto-linked to Arrow %s"), *GetName(), *Arrow->GetName());
                break;
            }
        }

        if (!PlayerSpawnArrow)
        {
            UE_LOG(LogTemp, Warning, TEXT("%s: No PlayerSpawnTransform arrow found!"), *GetName());
        }

    }

    LinkReadyDisplay();
    SetReady(false);
}

void ALobbyPlayerWindow::SpawnCharacter(APlayerController* PC)
{
    if (!PC) return;

    PlayerController = PC;
	auto CharacterClass = PC->GetPlayerState<AMainPlayerState>()->SelectedCharacterClass;

    if (IsValid(CurrentCharacter))
    {
        CurrentCharacter->Destroy();
        CurrentCharacter = nullptr;
    }


    // Check for valid class
    if (!CharacterClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: characterClass is not set!"), *GetName());
        return;

    }

    // Get spawn transform from the assigned arrow (fallback to actor location if missing)
    FTransform SpawnTransform = PlayerSpawnArrow
        ? PlayerSpawnArrow->GetComponentTransform()
        : GetActorTransform();

    // Always spawn, ignore collisions
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = nullptr;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // Spawn the actor
    ALobbyCharacterActor* NewCharacter = GetWorld()->SpawnActor<ALobbyCharacterActor>(
        CharacterClass,
        SpawnTransform,
        SpawnParams
    );

    if (IsValid(NewCharacter))
    {
        NewCharacter->AttachToComponent(
            PlayerSpawnArrow,
            FAttachmentTransformRules(EAttachmentRule::KeepWorld, true)
        );

        CurrentCharacter = NewCharacter;
        auto playerState = PlayerController->GetPlayerState<AMainPlayerState>();
        playerState->SetColor(CurrentCharacter->GetColor());

        LinkedSign->SetPlayerName(playerState->GetSteamPlayerName());

        /*
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                5.0f,
                FColor::Green,
                FString::Printf(TEXT("PlayerColor value: %f"), color)
            );
        }*/

        UE_LOG(LogTemp, Log, TEXT("%s: Spawned %s for player %s"),
            *GetName(), *NewCharacter->GetName(), *PC->GetName());

        AMainPlayerController* MainPC = Cast<AMainPlayerController>(PlayerController);
        if (!MainPC) return;

        ULobbyControllerComponent* LobbyComp = MainPC->FindComponentByClass<ULobbyControllerComponent>();
        if (!LobbyComp) return;
        
        SetReady(LobbyComp->GetIsReady());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("%s: Failed to spawn lobby actor!"), *GetName());
    }
}

void ALobbyPlayerWindow::Clear()
{
    PlayerController = nullptr;

    if (IsValid(CurrentCharacter))
    {
        CurrentCharacter->Destroy();
        CurrentCharacter = nullptr;
        LinkedSign->ResetSign();

    }
    SetReady(false);
}

void ALobbyPlayerWindow::LinkReadyDisplay()
{
    if (Tags.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s has no tag! Cannot link to ready sign."), *GetName());
        return;
    }

    FName MyTag = Tags[0];

    for (TActorIterator<AReadyDisplay> It(GetWorld()); It; ++It)
    {
        AReadyDisplay* OtherDisplay = *It;

        if (OtherDisplay->ActorHasTag(MyTag))
        {
            LinkedSign = OtherDisplay;
            UE_LOG(LogTemp, Log, TEXT("%s linked to ReadyDisplay: %s (tag %s)"),
                *GetName(), *OtherDisplay->GetName(), *MyTag.ToString());
            break;
        }
    }
}

APlayerController* ALobbyPlayerWindow::GetPC()
{
    return PlayerController;
}

void ALobbyPlayerWindow::SetReady(bool isReady)
{
    // Only the server should toggle state
    if (HasAuthority())
    {
        IsReady = isReady;

        // Call manually on the server (clients will get OnRep)
        OnRep_IsReady();
    }
}

void ALobbyPlayerWindow::OnRep_IsReady()
{
    // Update visuals on both server and clients
    if (LinkedSign)
    {
        LinkedSign->SetReadyState(IsReady);
    }

    UE_LOG(LogTemp, Log, TEXT("Window %s ready state replicated: %s"),
        *GetName(), IsReady ? TEXT("true") : TEXT("false"));
}

void ALobbyPlayerWindow::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ALobbyPlayerWindow, IsReady);
}