// Copyright Abonneer, Inc. All Rights Reserved.
#include "ReadyDisplay.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "StarterProject/UI/ScrollTextWidget.h"

AReadyDisplay::AReadyDisplay()
{
	PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
}

void AReadyDisplay::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

}
void AReadyDisplay::BeginPlay()
{
    Super::BeginPlay();

    UWidgetComponent* TextWidgetComp = FindComponentByClass<UWidgetComponent>();
    if (TextWidgetComp)
    {

        UUserWidget* Widget = TextWidgetComp->GetUserWidgetObject();
        if (!Widget) return;

        TextWidget = Cast< UScrollTextWidget>(Widget);


        // Use first numeric tag to get player index
        int32 PlayerIndex = 0;
        if (Tags.Num() > 0)
        {
            PlayerIndex = FCString::Atoi(*Tags[0].ToString());
        }
        if(PlayerText.IsEmpty())
            {
                 PlayerText = FString::Printf(TEXT("PLAYER %d"), PlayerIndex + 1);
            }
        
        TextWidget->SetScrollName(PlayerText);
    }


    if (!TargetMesh)
    {
        TargetMesh = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("SM_DisplayBoard")));
    }
    if (TargetMesh)
    {
        UE_LOG(LogTemp, Log, TEXT("Found Display Board: %s"), *TargetMesh->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Display Board not found!"));
    }

    if (TargetMesh && LightMaterial)
    {
        DynamicMaterial = UMaterialInstanceDynamic::Create(LightMaterial, this);
        TargetMesh->SetMaterial(0, DynamicMaterial);
        DynamicMaterial->SetVectorParameterValue(ColorParameterName, StartColor);
    }

    SetReadyState(IsReady);
}

void AReadyDisplay::SetReadyState(bool bReady)
{
    IsReady = bReady;

    if (DynamicMaterial)
    {
        float NewIntensity = IsReady ? ReadyIntensity : NotReadyIntensity;
        DynamicMaterial->SetScalarParameterValue(TEXT("Intensity"), NewIntensity);
    }

    // Always notify blueprint
    ReadyStatusChangedEvent(bReady);

    UE_LOG(LogTemp, Log, TEXT("%s is now %s"), *GetName(), IsReady ? TEXT("READY") : TEXT("NOT READY"));
}

void AReadyDisplay::OnRep_IsReady()
{
    SetReadyState(IsReady); // This calls BP event + updates material
}

void AReadyDisplay::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AReadyDisplay, IsReady);
    DOREPLIFETIME(AReadyDisplay, PlayerText);
}


void AReadyDisplay::SetPlayerName(FString playerName)
{
	PlayerText = playerName;

    if(HasAuthority())
    {
        TextWidget->SetScrollName(PlayerText);
    }
    //OnRep_PlayerText();
    //TextWidget->SetScrollName(PlayerText);
}
void AReadyDisplay::ResetSign()
{
    int32 PlayerIndex = FCString::Atoi(*Tags[0].ToString());
    PlayerText = FString::Printf(TEXT("PLAYER %d"), PlayerIndex + 1);
    TextWidget->SetScrollName(PlayerText);
    SetReadyState(false);
}


void AReadyDisplay::OnRep_PlayerText()
{

    if (!TextWidget)
    {
        UWidgetComponent* TextWidgetComp = FindComponentByClass<UWidgetComponent>();
        if (TextWidgetComp)
        {
            TextWidget = Cast<UScrollTextWidget>(TextWidgetComp->GetUserWidgetObject());
        }
    }
    if (TextWidget)
    {
        TextWidget->SetScrollName(PlayerText);
    }
}