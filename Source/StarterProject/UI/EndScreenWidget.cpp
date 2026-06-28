// Copyright Abonneer, Inc. All Rights Reserved.


#include "EndScreenWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "StarterProject/GameModes/EndScreenGameMode.h" 
#include "Framework/GameInstances/CustomGameInstanceBase.h"
void UEndScreenWidget::NativeConstruct()
{
    Super::NativeConstruct();

    /*if (Button_Replay)
    {
        Button_Replay->OnClicked.AddDynamic(this, &UEndScreenWidget::OnButton_ReplayClicked);
    }*/

    if (Button_Exit)
    {
        Button_Exit->OnClicked.AddDynamic(this, &UEndScreenWidget::OnButton_ExitClicked);

    }
}

void UEndScreenWidget::OnButton_ExitClicked()
{
    auto gameInstance = GetGameInstance<UCustomGameInstanceBase>();
    if (!gameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("Could not find gameinstance..."));
        return;
    }

    gameInstance->DestroySession();

    UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), MainMenu, true);

    UE_LOG(LogTemp, Log, TEXT("Player exited session!"));
}

//void UEndScreenWidget::OnButton_ReplayClicked()
//{
//
//}