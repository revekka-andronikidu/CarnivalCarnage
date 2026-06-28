// Copyright Abonneer, Inc. All Rights Reserved.


#include "EndScreenPlatform.h"
#include "Components/TextRenderComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values
AEndScreenPlatform::AEndScreenPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AEndScreenPlatform::BeginPlay()
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

    if(!TextComp)
	TextComp = FindComponentByClass<UTextRenderComponent>();
}

void AEndScreenPlatform::SpawnCharacter(APlayerController* PC)
{
    if (!PC) return;

    PlayerController = PC;

    if (IsValid(CurrentCharacter))
    {
        CurrentCharacter->Destroy();
        CurrentCharacter = nullptr;
    }

    //TODO:
    //Save char class in PC when selecting character in the lobby 
    //Get correct character here to spawn

    //Get name from PC and set the player name
   


    // Check for valid class
    if (!LobbyCharacterClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: LobbyCharacterClass is not set!"), *GetName());
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
    AActor* NewCharacter = GetWorld()->SpawnActor<AActor>(
        LobbyCharacterClass,
        SpawnTransform,
        SpawnParams
    );

    if (IsValid(NewCharacter))
    {
        CurrentCharacter = NewCharacter;

        UE_LOG(LogTemp, Log, TEXT("%s: Spawned %s for player %s"),
            *GetName(), *NewCharacter->GetName(), *PC->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("%s: Failed to spawn actor!"), *GetName());
    }
}