// Copyright Abonneer, Inc. All Rights Reserved.


#include "LobbyUIWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "StarterProject/GameModes/LobbyGameMode.h" 
#include "StarterProject/PlayerController/MainPlayerController.h"

void ULobbyUIWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Button_StartMatch)
    {
        Button_StartMatch->OnClicked.AddDynamic(this, &ULobbyUIWidget::OnButton_StartMatchClicked);

        //Only show start match button for the host
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC && PC->HasAuthority())
        {
            Button_StartMatch->SetVisibility(ESlateVisibility::Visible);
            Button_StartMatch->SetIsEnabled(false);
        }
        else
        {
            Button_StartMatch->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    if (Button_ToggleReady)
    {
        Button_ToggleReady->OnClicked.AddDynamic(this, &ULobbyUIWidget::OnButton_ReadyClicked);

        //Only show ready button for the clients, host is always ready
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC && PC->HasAuthority())
        {
            Button_ToggleReady->SetVisibility(ESlateVisibility::Collapsed);
        }
        else
        {
            Button_ToggleReady->SetVisibility(ESlateVisibility::Visible);
        }
    }
}

void ULobbyUIWidget::OnButton_StartMatchClicked()
{
    OnStartClickedEvent();
}

void ULobbyUIWidget::OnButton_ReadyClicked()
{
    //APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    //if (!PC) return;

    //ULobbyControllerComponent* LobbyComp = PC->FindComponentByClass<ULobbyControllerComponent>();
    //if (LobbyComp)
    //{
    //    LobbyComp->RequestReadyToggle();
    //}
   
    //UE_LOG(LogTemp, Log, TEXT("Ready toggle pressed! - requesting server..."));
}    


   
void ULobbyUIWidget::ToggleStartEnabled(bool enabled)
{
    IsEveryoneReady = enabled;
    ReadyStatusChangedEvent();
    Button_StartMatch->SetIsEnabled(enabled);
}

void ULobbyUIWidget::OnSequenceFinished()
{
    UWorld* World = GetWorld();
    if (!World) return;

    AGameModeBase* GameMode = UGameplayStatics::GetGameMode(World);
    if (!GameMode) return;

    ALobbyGameMode* LobbyGM = Cast<ALobbyGameMode>(GameMode);
    if (LobbyGM)
    {
        LobbyGM->StartGame();
    }

    UE_LOG(LogTemp, Log, TEXT("StartMatch triggered!"));
}