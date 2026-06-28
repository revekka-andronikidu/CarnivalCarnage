// Copyright Abonneer, Inc. All Rights Reserved.


#include "LobbyCharacterActor.h"

// Sets default values
ALobbyCharacterActor::ALobbyCharacterActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicateMovement(true); //to allow animations to replicate

}

// Called when the game starts or when spawned
void ALobbyCharacterActor::BeginPlay()
{
    Super::BeginPlay();

    if (!MeshComp)
    {
        MeshComp = FindComponentByClass<USkeletalMeshComponent>();
    }

    if (MeshComp)
    {
        UMaterialInstance* MID = Cast<UMaterialInstance>(MeshComp->GetMaterial(1));
        if (MID)
        {
            UE_LOG(LogTemp, Log, TEXT("Found MID: %s"), *MID->GetName());

            float Value;
            if (MID->GetScalarParameterValue(FName("Player_color"), Value))
            {
                PlayerColor = Value;
                UE_LOG(LogTemp, Log, TEXT("Read PlayerColor from MID: %f"), PlayerColor);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("PlayerColor not found"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("PlayerColor MI not found"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerColor MESH not found"));
    }
}

float ALobbyCharacterActor::GetColor()
{
    return PlayerColor;
}



